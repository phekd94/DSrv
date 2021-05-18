
#pragma once

//-------------------------------------------------------------------------------------------------
/*
DESCRITION: pure virtual template class for interface and concrete classes
TODO:
FIXME:
DANGER:
NOTE:

Thread safety: YES
Reentrance: YES
*/

//-------------------------------------------------------------------------------------------------
#include <cstdint>  // integer types
#include <utility>  // std::pair; std::move

#include "DSrv/other/printDebug.h"  // PRINT_DBG, PRINT_ERR

//-------------------------------------------------------------------------------------------------
namespace DSrv
{

//-------------------------------------------------------------------------------------------------
// Test class definition
template <typename Storage> class Base_test;

//-------------------------------------------------------------------------------------------------
template <typename Storage>
class Base : protected Storage
{
	friend class Base_test<Storage>;

public:

	// Data type for send data method (pointer + size)
	typedef  std::pair<const uint8_t *, uint32_t>  Data_send;

	// Data type for data parser method (pointer + size)
	typedef  Data_send  Data_parser;

protected:

	// Interface type enumeration
	const enum class InterfaceType
	{
		NONE, PACKET, SERIAL
	} m_interfaceType;

	// Constructor
	explicit Base(const InterfaceType interfaceType = InterfaceType::NONE) 
		: m_interfaceType(interfaceType)
	{
		PRINT_DBG(m_debug, "");
	}

	// Destructor
	virtual ~Base()
	{
		PRINT_DBG(m_debug, "");
	}
	
	// Copy constructor
	Base(const Base &) = default;

	// Move constructor
	Base(Base && obj) 
		: Storage(std::move(obj)), m_interfaceType(obj.m_interfaceType), m_debug(obj.m_debug)
	{
		PRINT_DBG(m_debug, "Move constructor");
	}
	
	// Override an asignment operator
	Base & operator=(const Base & obj)
	{
		if (&obj == this)
		{
			PRINT_DBG(m_debug, "Base: Self-assignment");
			return *this;
		}
		
		// Copy Storage part
		Storage::operator=(obj);
	
		// Copy all fields
		m_debug = obj.m_debug;
		*const_cast<InterfaceType *>(&m_interfaceType) = obj.m_interfaceType;
		
		PRINT_DBG(m_debug, "Base");
		
		return *this;
	}

	// Enables debug messages
	void setDebug(const bool d_base, const bool d_storage) noexcept
	{
		m_debug = d_base;
		Storage::setDebug(d_storage);
	}

	// Sends data (pure virtual function)
	// (protocol class should realize this function)
	virtual int32_t sendData(const Data_send data) noexcept = 0;
	
	// Receives data (pure virtual function)
	// (protocol class should realize this function)
	virtual int32_t receiveData() noexcept = 0;

	// Parser of the accepted data (pure virtual function)
	// (concrete class should realize this function)
	virtual int32_t dataParser(Data_parser data) noexcept = 0;

private:

	// Enable debug messages
	bool m_debug {true};
};

//-------------------------------------------------------------------------------------------------
} // namespace DSrv
