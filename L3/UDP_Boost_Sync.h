
#pragma once

//-------------------------------------------------------------------------------------------------
/*
DESCRITION: class implements work with UDP packets by using Boost library
TODO:
FIXME:
DANGER:
 * If CRC is needed buffer should be more than size on sizeof(uint32_t) 
NOTE:

Thread safety: YES
Reentrance: YES
*/

//-------------------------------------------------------------------------------------------------
#include <cstdint>         // integer types
#include <exception>       // std::exception
#include "boost/asio.hpp"  // Boost.Asio library
#include "boost/crc.hpp"   // Boost.CRC library

#include "DSrv/other/printDebug.h"  // PRINT_DBG, PRINT_ERR

//-------------------------------------------------------------------------------------------------
namespace DSrv 
{

//-------------------------------------------------------------------------------------------------
// Test class definition
template <typename Storage, template <typename T> class Base> class UDP_Boost_Sync_test;

//-------------------------------------------------------------------------------------------------
template <typename Storage, template <typename T> class Base>
class UDP_Boost_Sync : public Base<Storage>
{

	friend class UDP_Boost_Sync_test<Storage, Base>;
	
public:
	
	// Maximal lenght of UDP packet
	constexpr static const uint32_t UDP_MAX_LENGTH {1500};
	
	// Starts the communication
	int32_t start(const uint16_t port);
	
	// Stops the communication
	int32_t stop();

protected:

	// Constructor
	UDP_Boost_Sync() 
		: Base<Storage>(Base<Storage>::InterfaceType::PACKET), 
		  m_socket(m_io_context)
	{
		PRINT_DBG(m_debug, "");
	}
	
	// Destructor
	virtual ~UDP_Boost_Sync()
	{
		stop();
	
		PRINT_DBG(m_debug, "");
	}
	
	// Copy constructor; initialization object will not be started; Base<Storage> part is copied
	UDP_Boost_Sync(const UDP_Boost_Sync & obj);

	// Move constructor	
	UDP_Boost_Sync(UDP_Boost_Sync && obj);
	
	// Override an assignment operator; object will not be started; Base<Storage> part is copied
	UDP_Boost_Sync & operator=(const UDP_Boost_Sync & obj);
	
	// Sends data to the host and port
	virtual int32_t sendData(typename Base<Storage>::Data_send data,
	                         const std::string host, const uint16_t port, 
	                         const bool crc = false) noexcept;
	
	// Receives data (override method)
	virtual int32_t receiveData() noexcept override final;
	
	// Enables debug messages
	void setDebug(const bool d_usart, const bool d_base, const bool d_storage) noexcept
	{
		m_debug = d_usart;
		Base<Storage>::setDebug(d_base, d_storage);
	}

private:

	// IO context
	boost::asio::io_context m_io_context;
	
	// UDP socket
	boost::asio::ip::udp::socket m_socket;
	
	// Endpoint for receive
	boost::asio::ip::udp::endpoint m_endpointReceive;
	
	// Endpoint for send
	boost::asio::ip::udp::endpoint m_endpointSend;
	
	// Mutex
	std::mutex m_mutex;
	
	// CRC
	boost::crc_32_type m_crc;
	
	// Buffer size
	constexpr static const uint32_t m_bufferSize {UDP_MAX_LENGTH};
	
	// Buffer for receive data
	uint8_t m_bufferReceive[m_bufferSize];	
	
	// Enable debug messages
	bool m_debug {true};
	
	// Sends data (override method)
	virtual int32_t sendData(const typename Base<Storage>::Data_send data) noexcept 
	                                                                          override final;
};

//=================================================================================================
template <typename Storage, template <typename T> class Base>
int32_t UDP_Boost_Sync<Storage, Base>::
start(const uint16_t port)
{
	try {
		// Lock a mutex
		std::lock_guard<std::mutex> lock {m_mutex};
	
		// Open a socket
		m_socket.open(boost::asio::ip::udp::v4());
		
		// Bind a socket
		m_socket.bind(boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), port));
		
		PRINT_DBG(m_debug, "Communication is started (port: %u)", 
		                   static_cast<unsigned int>(port));
		
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
int32_t UDP_Boost_Sync<Storage, Base>::
stop()
{
	try {
		// Lock a mutex
		std::lock_guard<std::mutex> lock {m_mutex};
		
		// Get a current port
		uint16_t port {m_socket.local_endpoint().port()};
	
		// Close a socket
		m_socket.close();
		
		PRINT_DBG(m_debug, "Communication is stoped (port: %u)", 
		                   static_cast<unsigned int>(port));
		
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
UDP_Boost_Sync<Storage, Base>::
UDP_Boost_Sync(const UDP_Boost_Sync & obj)
	: Base<Storage>(obj),
	  m_socket(m_io_context),
	  m_debug(obj.m_debug)
{
	PRINT_DBG(m_debug, "Copy constructor");
}

//-------------------------------------------------------------------------------------------------
template <typename Storage, template <typename T> class Base>
UDP_Boost_Sync<Storage, Base>::
UDP_Boost_Sync(UDP_Boost_Sync && obj) 
	: Base<Storage>(std::move(obj)), 
	  m_socket(m_io_context),
	  m_endpointReceive(obj.m_endpointReceive),
	  m_endpointSend(obj.m_endpointSend),
	  m_debug(obj.m_debug)
{
	// Lock a mutex
	try {
		std::lock_guard<std::mutex> lock {m_mutex};
	}
	catch (std::system_error & err)
	{
		PRINT_ERR("Move: Exception from mutex.lock() has been occured: %s", err.what());
		return;
	}
	
	// Error code for Boost library
	boost::system::error_code ec;
	
	// Get a local address and port
	const boost::asio::ip::udp::endpoint ep {obj.m_socket.local_endpoint(ec)};

	// Start new interface object
	if (obj.m_socket.is_open() == true)
	{
		PRINT_DBG(m_debug, "Move: Stop a socket being moved");
		
		// Stop old interface object
		obj.stop();
		
		if (!ec)
		{
			start(ep.port());
		}
	}
	
	PRINT_DBG(m_debug, "Move constructor");
}

//-------------------------------------------------------------------------------------------------
template <typename Storage, template <typename T> class Base>
UDP_Boost_Sync<Storage, Base> & UDP_Boost_Sync<Storage, Base>::
operator=(const UDP_Boost_Sync & obj)
{
	// Self-assignment check
	if (&obj == this)
	{
		PRINT_DBG(m_debug, "UDP_Boost_Sync: Self-assignment");
		return *this;
	}
	
	// Copy Base<Storage> part
	Base<Storage>::operator=(obj);
	
	// Copy field
	m_debug = obj.m_debug;
	
	PRINT_DBG(m_debug, "UDP_Boost_Sync");
	
	return *this;
}

//-------------------------------------------------------------------------------------------------
template <typename Storage, template <typename T> class Base>
int32_t UDP_Boost_Sync<Storage, Base>::
sendData(const typename Base<Storage>::Data_send data) noexcept
{
	try {
		// Lock a mutex
		std::lock_guard<std::mutex> lock {m_mutex};
		
		// Send data
		m_socket.send_to(boost::asio::buffer(data.first, data.second), m_endpointSend);
		
		PRINT_DBG(m_debug, "Data is send to %s (port = %u) (size = %u)",
		                   m_endpointSend.address().to_string().c_str(),
		                   static_cast<unsigned int>(m_endpointSend.port()),
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
int32_t UDP_Boost_Sync<Storage, Base>::
sendData(typename Base<Storage>::Data_send data, 
         const std::string host, const uint16_t port, const bool crc) noexcept
{
	try {
		// Set endpoint for send
		m_endpointSend = boost::asio::ip::udp::endpoint(
		                      boost::asio::ip::address::from_string(host), port);
		
		// Add CRC
		if (crc == true)
		{
			// Calculate a CRC
			m_crc.process_bytes(data.first, data.second);
			
			// Add CRC to data
			*reinterpret_cast<uint32_t *>(
			   const_cast<uint8_t *>(data.first + data.second)) = m_crc.checksum();
			data.second += sizeof(uint32_t);
			
			PRINT_DBG(m_debug, "CRC for send data: 0x%x", m_crc.checksum());
		}
		
		// Send data
		if (sendData(data) != 0)
		{
			PRINT_ERR("sendData(data)");
			return -1;
		}
		
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
int32_t UDP_Boost_Sync<Storage, Base>::
receiveData() noexcept
{
	try {
		// Lock a mutex
		std::lock_guard<std::mutex> lock {m_mutex};
	
		// Check of data availability
		if (m_socket.available() == 0)
			return 1;
		
		// Read all available packets
		while (m_socket.available() != 0) 
		{
			// Receive data
			uint32_t receiveBytes = m_socket.receive_from(
			        boost::asio::buffer(m_bufferReceive, m_bufferSize), m_endpointReceive);
			        
			PRINT_DBG(m_debug, "Data is received from %s (port = %u) (size = %u)", 
			                   m_endpointReceive.address().to_string().c_str(),
			                   static_cast<unsigned int>(m_endpointReceive.port()), 
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
		
		return 0;
	}
	catch (std::exception & ex)
	{
		PRINT_ERR("%s", ex.what());
		return -1;
	}
}

//-------------------------------------------------------------------------------------------------
} // namespace DSrv
