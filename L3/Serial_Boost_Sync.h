
#pragma once

//-------------------------------------------------------------------------------------------------
/*
DESCRITION: class implements work with serial port devices by using Boost library
TODO:
FIXME:
DANGER: 
NOTE:

Thread safety: TODO
Reentrance: TODO
*/

//-------------------------------------------------------------------------------------------------
#include <cstdint>         // integer types
#include <exception>       // std::exception
#include <string>          // std::string
#include "boost/asio.hpp"  // Boost.Asio library

#include "DSrv/other/printDebug.h"  // PRINT_DBG, PRINT_ERR

//-------------------------------------------------------------------------------------------------
namespace DSrv 
{

//-------------------------------------------------------------------------------------------------
// Test class definition
template <typename Storage, template <typename T> class Base> class Serial_Boost_Sync_test;

//-------------------------------------------------------------------------------------------------
template <typename Storage, template <typename T> class Base>
class Serial_Boost_Sync : public Base<Storage>
{

	friend class Serial_Boost_Sync_test<Storage, Base>;
	
public:
	
	// Maximal lenght of data for send/receive data via serial port
	constexpr static const uint32_t SERIAL_MAX_LENGTH {50};
	
	// Starts the communication
	int32_t start(const std::string & device);
	
	// Stops the communication
	int32_t stop();
	
protected:

	// Constructor
	Serial_Boost_Sync() 
		: Base<Storage>(Base<Storage>::InterfaceType::SERIAL), 
		  m_serial_port(m_io_context)
	{
		PRINT_DBG(m_debug, "");
	}
	
	// Destructor
	virtual ~Serial_Boost_Sync()
	{
		stop();
	
		PRINT_DBG(m_debug, "");
	}
	
	/*// Copy constructor; initialization object will not be started; Base<Storage> part is copied
	Serial_Boost_Sync(const Serial_Boost_Sync & obj);

	// Move constructor	
	Serial_Boost_Sync(Serial_Boost_Sync && obj);
	
	// Override an assignment operator; object will not be started; Base<Storage> part is copied
	Serial_Boost_Sync & operator=(const Serial_Boost_Sync & obj);*/
	
	// Sends data (override method)
	virtual int32_t sendData(const typename Base<Storage>::Data_send data) noexcept 
	                                                                          override final;
	
	// Receives data (override method)
	virtual int32_t receiveData() noexcept override final;
	
	// Enables debug messages
	void setDebug(const bool d_interface, const bool d_base, const bool d_storage) noexcept
	{
		m_debug = d_interface;
		Base<Storage>::setDebug(d_base, d_storage);
	}
	
private:

	// IO context
	boost::asio::io_context m_io_context;
	
	// Serial port
	boost::asio::serial_port m_serial_port;
	
	// Device name
	std::string m_deviceName {"null"};
	
	// Mutex
	std::mutex m_mutex;
	
	// Buffer size
	constexpr static const uint32_t m_bufferSize {SERIAL_MAX_LENGTH};
	
	// Buffer for receive data
	uint8_t m_bufferReceive[m_bufferSize];	
	
	// Enable debug messages
	bool m_debug {true};
};

//=================================================================================================
template <typename Storage, template <typename T> class Base>
int32_t Serial_Boost_Sync<Storage, Base>::
start(const std::string & device)
{
	try {
		// Lock a mutex
		std::lock_guard<std::mutex> lock {m_mutex};
	
		// Open a serial port
		m_serial_port.open(device);
		
		// Set the device name
		m_deviceName = device;
		
		// Set the handler for receive data
		/*boost::asio::async_read(m_serial_port,
		                        boost::asio::buffer(m_bufferReceive, m_bufferSize),
		                        ...);*/
		
		PRINT_DBG(m_debug, "Communication is started (device name: %s)", m_deviceName.c_str());
		
		return 0;
	}
	catch (std::exception & ex)
	{
		PRINT_ERR("%s", ex.what());
		return -1;
	}
}

//-------------------------------------------------------------------------------------------------
template <typename Storage, template <typename T> class Base>
int32_t Serial_Boost_Sync<Storage, Base>::
stop()
{
	try {
		// Lock a mutex
		std::lock_guard<std::mutex> lock {m_mutex};
			
		// Close a serial port
		m_serial_port.close();
		
		PRINT_DBG(m_debug, "Communication is stoped (device name: %s)", m_deviceName.c_str());
		
		// Reset the device name
		m_deviceName = "null";
		
		return 0;
	}
	catch (std::exception & ex)
	{
		PRINT_ERR("%s", ex.what());
		return -1;
	}
}

//-------------------------------------------------------------------------------------------------
template <typename Storage, template <typename T> class Base>
int32_t Serial_Boost_Sync<Storage, Base>::
sendData(const typename Base<Storage>::Data_send data) noexcept
{
	try {
		// Lock a mutex
		std::lock_guard<std::mutex> lock {m_mutex};
		
		// Send data
		boost::asio::write(m_serial_port, boost::asio::buffer(data.first, data.second));
		
		PRINT_DBG(m_debug, "Data is send to %s (size = %u)",
		                   m_deviceName.c_str(),
		                   static_cast<unsigned int>(data.second));
		
		return 0;
	}
	catch (std::exception & ex)
	{
		PRINT_ERR("%s", ex.what());
		return -1;
	}
}

//-------------------------------------------------------------------------------------------------
template <typename Storage, template <typename T> class Base>
int32_t Serial_Boost_Sync<Storage, Base>::
receiveData() noexcept
{
	try {
		// Lock a mutex
		std::lock_guard<std::mutex> lock {m_mutex};
			
		// Read all available packets
		while (true) 
		{
			// Receive data
			auto receiveBytes = m_serial_port.read_some(boost::asio::buffer(m_bufferReceive, 
			                                                                m_bufferSize));
			if (receiveBytes > 0)
			{
				PRINT_DBG(m_debug, "Data is received from %s (size = %u)", 
					               m_deviceName.c_str(), 
					               static_cast<unsigned int>(receiveBytes));
				
				// Parse data
				if (this->dataParser(
					     typename Base<Storage>::Data_parser(m_bufferReceive, receiveBytes)) != 0)
				{
					PRINT_ERR("dataParser()");
				
					// Clear data
					if (Storage::clearData() != 0)
					{
						PRINT_ERR("clearData()");
					}
					return -1;
				}
			}
			else
			{
				return 0;
			}
		}
	}
	catch (std::exception & ex)
	{
		PRINT_ERR("%s", ex.what());
		return -1;
	}
}

//-------------------------------------------------------------------------------------------------
} // namespace DSrv
