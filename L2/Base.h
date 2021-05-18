
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
	enum class InterfaceType
	{
		PACKET, SERIAL
	};

	// Constructor
	explicit Base(const uint32_t bufferSize) : Storage(bufferSize)
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
	Base(Base && obj) : Storage(std::move(obj)), m_debug(obj.m_debug)
	{
		PRINT_DBG(m_debug, "Move constructor");
	}
	
	// Override an asignment operator
	Base & operator=(const Base &) = default;

	// Enables debug messages
	void setDebug(const bool d, const bool d_storage) noexcept
	{
		m_debug = d;
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
