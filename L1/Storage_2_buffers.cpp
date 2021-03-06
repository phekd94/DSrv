
#include "Storage_2_buffers.h"

#include <cstring>       // std::memcpy
#include <utility>       // std::swap
#include <system_error>  // std::system_error
#include <utility>       // std::move
#include <algorithm>     // std::copy

#include "DSrv/other/printDebug.h"  // PRINT_DBG, PRINT_ERR

//-------------------------------------------------------------------------------------------------
using namespace DSrv;

//-------------------------------------------------------------------------------------------------
Storage_2_buffers::Streambuf::Streambuf(Storage_2_buffers::Streambuf && obj)
{
	// Set pointer from moving object
	setPointers(obj.eback(), obj.gptr(), obj.egptr());

	PRINT_DBG(true, "Move constructor");
}

//-------------------------------------------------------------------------------------------------
Storage_2_buffers::Storage_2_buffers() : m_istream(&m_streambuf)
{
	PRINT_DBG(m_debug, "");
}

//-------------------------------------------------------------------------------------------------
Storage_2_buffers::~Storage_2_buffers()
{
	PRINT_DBG(m_debug, "");
}

//-------------------------------------------------------------------------------------------------
Storage_2_buffers::Storage_2_buffers(const Storage_2_buffers & obj) : m_istream(&m_streambuf)
{
	try {
		// Lock a mutex being copied
		std::lock_guard<std::mutex> lock {const_cast<Storage_2_buffers &>(obj).m_mutex};
	
		// Check the new buffer memory
		if (obj.m_completeData.get() == nullptr || obj.m_fillingData.get() == nullptr)
		{
			PRINT_ERR("Bad new buffer memory");
			return;
		}
	
		// Set buffers parameters
		*const_cast<uint32_t *>(&m_dataSize) = obj.m_dataSize;
		m_fillingIndex = obj.m_fillingIndex;
		m_completeSize = obj.m_completeSize;
		m_debug = obj.m_debug;

		// Allocate a memory for the data
		allocate_(m_dataSize);
	
		// Copy data (+ 1 for last element)
		std::copy(obj.m_completeData.get(), obj.m_completeData.get() + m_dataSize + 1, 
			      m_completeData.get());
		std::copy(obj.m_fillingData.get(), obj.m_fillingData.get() + m_dataSize + 1, 
			      m_fillingData.get());
	
		// Set pointers to the input buffer
		m_streambuf.setPointers(reinterpret_cast<char *>(m_fillingData.get()), 
			                    reinterpret_cast<char *>(m_fillingData.get()), 
			                    reinterpret_cast<char *>(m_fillingData.get() + m_fillingIndex));
	
		PRINT_DBG(m_debug, "Copy constructor");
	}
	catch (std::system_error & err)
	{
		PRINT_ERR("Exception from mutex.lock() has been occured: %s", err.what());
		return;
	}
}

//-------------------------------------------------------------------------------------------------
Storage_2_buffers::Storage_2_buffers(Storage_2_buffers && obj) 
	: m_streambuf(std::move(obj.m_streambuf)), 
	  m_istream(&m_streambuf), 
	  m_completeData(std::move(obj.m_completeData)),
	  m_fillingData(std::move(obj.m_fillingData)),
	  m_dataSize(obj.m_dataSize),
	  m_fillingIndex(obj.m_fillingIndex),
	  m_completeSize(obj.m_completeSize),
	  m_debug(obj.m_debug)
{
	PRINT_DBG(m_debug, "Move constructor");
}

//-------------------------------------------------------------------------------------------------
Storage_2_buffers & Storage_2_buffers::operator=(const Storage_2_buffers & obj)
{
	// Self-assignment check
	if (&obj == this)
	{
		PRINT_DBG(m_debug, "Storage_2_buffers: Self-assignment");
		return *this;
	}
	
	try {
		// Lock mutexes
		std::lock_guard<std::mutex> lock {m_mutex};
		std::lock_guard<std::mutex> lock_obj {const_cast<Storage_2_buffers &>(obj).m_mutex};
	
		// Check the new buffer memory
		if (obj.m_completeData.get() == nullptr || obj.m_fillingData.get() == nullptr)
		{
			PRINT_ERR("Bad new buffer memory");
		
			// Return bad buffer
			return const_cast<Storage_2_buffers &>(obj);
		}
	
		// Data sizes are not match
		if (obj.m_dataSize != m_dataSize)
		{
			PRINT_DBG(m_debug, "Delete old data and allocate memory for new data");
		
			// Delete old buffers
			deallocate_();
		
			// New data size member
			*const_cast<uint32_t *>(&m_dataSize) = obj.m_dataSize;
		
			allocate_(m_dataSize);
		}
	
		// New buffer parameters
		m_fillingIndex = obj.m_fillingIndex;
		m_completeSize = obj.m_completeSize;
		m_debug = obj.m_debug;
	
		// Copy data (+ 1 for last element)
		std::copy(obj.m_completeData.get(), obj.m_completeData.get() + m_dataSize + 1, 
			      m_completeData.get());
		std::copy(obj.m_fillingData.get(), obj.m_fillingData.get() + m_dataSize + 1, 
			      m_fillingData.get());
	
		// Set pointers to the input buffer
		m_streambuf.setPointers(reinterpret_cast<char *>(m_fillingData.get()), 
			                    reinterpret_cast<char *>(m_fillingData.get()), 
			                    reinterpret_cast<char *>(m_fillingData.get() + m_fillingIndex));
	
		PRINT_DBG(m_debug, "Storage_2_buffers");

		return *this;
	}
	catch (std::system_error & err)
	{
		PRINT_ERR("Exception from mutex.lock() has been occured: %s", err.what());
		
		// Return bad buffer
		return const_cast<Storage_2_buffers &>(obj);
	}
}

//-------------------------------------------------------------------------------------------------
int32_t Storage_2_buffers::allocate(const uint32_t size) noexcept
{
	try {
		// Lock a mutex
		std::lock_guard<std::mutex> lock {m_mutex};
		
		// Delete old data
		deallocate_();
	
		// Allocate a memory for the data
		if (allocate_(size) != 0)
		{
			PRINT_ERR("allocate_(size)");
			return -1;
		}
	
		// Set data size member
		*const_cast<uint32_t *>(&m_dataSize) = size;

		PRINT_DBG(m_debug, "");
	
		return 0;
	}
	catch (std::system_error & err)
	{
		PRINT_ERR("Exception from mutex.lock() has been occured: %s", err.what());
		return -1;
	}
}

//-------------------------------------------------------------------------------------------------
int32_t Storage_2_buffers::deallocate() noexcept
{
	try {
		// Lock a mutex
		std::lock_guard<std::mutex> lock {m_mutex};
		
		// Delete data
		deallocate_();
	
		PRINT_DBG(m_debug, "");
	
		return 0;
	}
	catch (std::system_error & err)
	{
		PRINT_ERR("Exception from mutex.lock() has been occured: %s", err.what());
		return -1;
	}
}

//-------------------------------------------------------------------------------------------------
int32_t Storage_2_buffers::setData(Data_set data) noexcept
{
	// Check the incoming parameter
	if (nullptr == data.first)
	{
		PRINT_ERR("nullptr == data.first");
		return -1;
	}

	// Check the data pointers
	if (nullptr == m_completeData.get() || nullptr == m_fillingData.get())
	{
		PRINT_ERR("Memory for data have not been allocated");
		return -1;
	}

	try {
		// Lock a mutex
		std::lock_guard<std::mutex> lock {m_mutex};

		// Check the size of the input data
		if (m_fillingIndex + data.second > m_dataSize)
		{
			PRINT_ERR("Size of the data is too much (size = %lu, fillingIndex = %lu)",
				      static_cast<unsigned long>(data.second),
				      static_cast<unsigned long>(m_fillingIndex));
			return -1;
		}

		PRINT_DBG(m_debug, "Add the data(0x%p) with size(%5lu) to the 0x%p with size(%5lu)",
			               data.first,
			               static_cast<unsigned long>(data.second),
			               m_fillingData.get(),
			               static_cast<unsigned long>(m_fillingIndex));

		// Add the data and add the size to the fillingIndex
		std::memcpy(m_fillingData.get() + m_fillingIndex, data.first, data.second);
		m_fillingIndex += data.second;
	
		// Set pointers to the input buffer
		m_streambuf.setPointers(reinterpret_cast<char *>(m_fillingData.get()), 
			                    reinterpret_cast<char *>(m_fillingData.get()), 
			                    reinterpret_cast<char *>(m_fillingData.get() + m_fillingIndex));
	
		return 0;
	}
	catch (std::system_error & err)
	{
		PRINT_ERR("Exception from mutex.lock() has been occured: %s", err.what());
		return -1;
	}
}

//-------------------------------------------------------------------------------------------------
int32_t Storage_2_buffers::getData(Data_get & data) noexcept
{
	// Check the data pointers
	if (nullptr == m_completeData.get() || nullptr == m_fillingData.get())
	{
		PRINT_ERR("Memory for data have not been allocated");
		data.first = nullptr;
		data.second = 0;
		return -1;
	}

	try {
		// Lock a mutex
		std::lock_guard<std::mutex> lock {m_mutex};
		
		PRINT_DBG(m_debug, "Data for geter: 0x%p with size(%5lu)",
			                m_completeData.get(),
			                static_cast<unsigned long>(m_completeSize));	
	
		// Set the data and the size
		data.first = m_completeData.get();
		data.second = m_completeSize;
		
		return 0;
	}
	catch (std::system_error & err)
	{
		PRINT_ERR("Exception from mutex.lock() has been occured: %s", err.what());
		data.first = nullptr;
		data.second = 0;
		return -1;
	}
}

//-------------------------------------------------------------------------------------------------
int32_t Storage_2_buffers::clearData() noexcept
{
	try {
		// Lock a mutex
		std::lock_guard<std::mutex> lock {m_mutex};
	
		// Set the fillingIndex and the completeSize to 0
		m_fillingIndex = 0;
		m_completeSize = 0;
	
		// Set pointers to the input buffer
		m_streambuf.setPointers(reinterpret_cast<char *>(m_fillingData.get()), 
			                    reinterpret_cast<char *>(m_fillingData.get()), 
			                    reinterpret_cast<char *>(m_fillingData.get()));

		PRINT_DBG(m_debug, "Data is clear");

		return 0;
	}
	catch (std::system_error & err)
	{
		PRINT_ERR("Exception from mutex.lock() has been occured: %s", err.what());
		return -1;
	}
}

//-------------------------------------------------------------------------------------------------
int32_t Storage_2_buffers::completeData() noexcept
{
	// Check the data pointers
	if (nullptr == m_completeData.get() || nullptr == m_fillingData.get())
	{
		PRINT_ERR("Memory for data have not been allocated");
		return -1;
	}

	try {
		// Lock a mutex
		std::lock_guard<std::mutex> lock {m_mutex};

		// Exchange pointers
		std::swap(m_fillingData, m_completeData);

		// Set the completeSize
		m_completeSize = m_fillingIndex;

		// Set the fillingIndex to 0
		m_fillingIndex = 0;
	
		// Set pointers to the input buffer
		m_streambuf.setPointers(reinterpret_cast<char *>(m_fillingData.get()), 
			                    reinterpret_cast<char *>(m_fillingData.get()), 
			                    reinterpret_cast<char *>(m_fillingData.get()));

		PRINT_DBG(m_debug, "Data(0x%p) with size(%5lu) is complete",
			               m_completeData.get(),
			               static_cast<unsigned long>(m_completeSize));

		return 0;
	}
	catch (std::system_error & err)
	{
		PRINT_ERR("Exception from mutex.lock() has been occured: %s", err.what());
		return -1;
	}
}

//-------------------------------------------------------------------------------------------------
uint32_t Storage_2_buffers::completeDataCRC()
{
	m_crc.process_bytes(m_completeData.get(), m_completeSize);
	return m_crc.checksum();
}

//-------------------------------------------------------------------------------------------------
void Storage_2_buffers::clearIstream() noexcept
{
	try
	{
		m_istream.clear();
	} catch (std::exception & ex)
	{
		PRINT_ERR("istream::clear()");
	}
}

//-------------------------------------------------------------------------------------------------
int32_t Storage_2_buffers::allocate_(const uint32_t size) noexcept
{
	// Allocate a memory for the data (+ 1 for element for end pointer position of the Streambuf)
	m_completeData = std::unique_ptr<uint8_t []>(new (std::nothrow) uint8_t[size + 1]);
	if (nullptr == m_completeData.get())
	{
		PRINT_ERR("Can not allocate a memory (m_completeData)");
		return -1;
	}
	m_fillingData = std::unique_ptr<uint8_t []>(new (std::nothrow) uint8_t[size + 1]);
	if (nullptr == m_fillingData.get())
	{
		PRINT_ERR("Can not allocate a memory (m_fillingData)");
		return -1;
	}
	
	// Set pointers to the input buffer
	m_streambuf.setPointers(reinterpret_cast<char *>(m_fillingData.get()), 
	                        reinterpret_cast<char *>(m_fillingData.get()), 
	                        reinterpret_cast<char *>(m_fillingData.get()));
	
	PRINT_DBG(m_debug, "");
	
	return 0;
}

//-------------------------------------------------------------------------------------------------
void Storage_2_buffers::deallocate_() noexcept
{
	delete [] m_completeData.release();
	delete [] m_fillingData.release();

	PRINT_DBG(m_debug, "");
}
