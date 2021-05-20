
#pragma once

//-------------------------------------------------------------------------------------------------
/*
DESCRITION: class with kaitai parser
TODO:
FIXME:
DANGER:
NOTE:

Thread safety: YES
Reentrance: YES
*/

//-------------------------------------------------------------------------------------------------
#include <cstdint>                // integer types
#include <memory>                 // std::unique_ptr
#include <chrono>                 // time library
#include "kaitai/kaitaistream.h"  // kaitai::kstream

#include "DSrv/other/printDebug.h"  // PRINT_DBG, PRINT_ERR

//-------------------------------------------------------------------------------------------------
namespace DSrv
{

//-------------------------------------------------------------------------------------------------
template <typename Storage, 
          template <typename T> class Base,
          template <typename T_1, template <typename Y> class T_2> class Interface,
          typename KaitaiParser>
class Concrete : public Interface<Storage, Base>
{
	
public:

	// Constructor
	Concrete() : m_kaitaiStream(Storage::getIstreamPointer())
	{
		PRINT_DBG(true, "");
	}
	
	// Destructor
	virtual ~Concrete()
	{
		PRINT_DBG(true, "");
	}

	// Loop for the server
	void loop(std::chrono::milliseconds period) noexcept;
	
	// Flag for stop the loop
	volatile bool m_stopLoop {false};
	
private:

	// Parser of the accepted data (override method)
	virtual int32_t dataParser(typename Base<Storage>::Data_parser data) noexcept override final;

	// Kaitai stream
	kaitai::kstream m_kaitaiStream;
	
	// Kaitai parser
	std::unique_ptr<KaitaiParser> m_kaitaiParser {nullptr};
};
//=================================================================================================
template <typename Storage, 
          template <typename T> class Base, 
          template <typename T_1, template <typename Y> class T_2> class Interface,
          typename KaitaiParser>
void Concrete<Storage, Base, Interface, KaitaiParser>::
loop(std::chrono::milliseconds period) noexcept
{
	// Allocate
	Storage::allocate(50);
	
	// Start
	Interface<Storage, Base>::start(50000);
	
	// Loop
	uint8_t i {0};
	PRINT_DBG(true, "Loop start");
	while (false == m_stopLoop)
	{
		// Send to itself
		uint8_t data[] {1, i++, 'H', 'W', '\0'};
		if (Interface<Storage, Base>::sendData(
		         typename Base<Storage>::Data_send(data, sizeof(data) / sizeof(uint8_t)), 
		         "0.0.0.0", 50000) != 0)
		{
			PRINT_ERR("sendData()");
			break;
		}
		
		auto start {std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::high_resolution_clock::now().time_since_epoch()
			)};
		
		// Wait
		while (std::chrono::duration_cast<std::chrono::milliseconds>(
				std::chrono::high_resolution_clock::now().time_since_epoch()
			  ) - start < period);
		
		// Receive data
		if (Interface<Storage, Base>::receiveData() < 0)
		{
			PRINT_ERR("receiveData()");
			break;
		}
	}
	
	PRINT_DBG(true, "Loop end");
}

//-------------------------------------------------------------------------------------------------
template <typename Storage, 
          template <typename T> class Base, 
          template <typename T_1, template <typename Y> class T_2> class Interface,
          typename KaitaiParser>
int32_t Concrete<Storage, Base, Interface, KaitaiParser>::
dataParser(typename Base<Storage>::Data_parser data) noexcept
{
	// Print data
	// for (uint32_t i = 0; i < data.second; ++i)
	//	PRINT_DBG(true, "[%u]: %u ", 
	//	                static_cast<unsigned int>(i),
	//	                static_cast<unsigned int>(data.first[i]));
	
	// TODO:
	/*if (... == Interface::type)
	{
	}
	else
	{
	}*/

	// Set data to buffer
	if (Storage::setData(typename Storage::Data_set(data.first, data.second)) != 0)
	{
		PRINT_ERR("setData()");
		return -1;
	}
	
	try
	{
		// Apply parser
		delete m_kaitaiParser.release();
		m_kaitaiParser = std::unique_ptr<KaitaiParser>(new KaitaiParser(&m_kaitaiStream));
	} catch (std::exception & ex)
	{
		PRINT_ERR("KaitaiParser or new: %s", ex.what());
		Storage::clearIstream();
		Storage::clearData();
		return 1;
	}
	
	// Complete data
	if (Storage::completeData() != 0)
	{
		PRINT_ERR("completeData()");
		return -1;
	}
	
	// Get data
	// typename Storage::Data_get dataGet;
	// Storage::getData(dataGet);
	
	return 0;
}

//-------------------------------------------------------------------------------------------------
} // namespace DSrv

/*
template <typename Storage, 
          template <typename T> class Base,
          template <typename T_1, template <typename Y> class T_2> class Interface, // interface member constexpr
          typename KaitaiParser>
class Concrete : public Interface<Storage, Base>
{

public:
	
	KaitaiParser * kp;
	
	if_constexpr (Interface::type)
	{
	}
	else
	{
	}

}

// using: Concrete<...>.kp->id or data
*/

