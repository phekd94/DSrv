
#pragma once

//-------------------------------------------------------------------------------------------------
/*
DESCRITION: class for storing data
TODO:
 * m_streambuf.setPointers() does not run when error is occured
 * mutex test
FIXME:
DANGER:
NOTE:

Thread safety: YES, but NO for the move constructor
Reentrance: YES, but take into account that only two buffers are available
*/

//-------------------------------------------------------------------------------------------------
#include <cstdint>        // integer types
#include <mutex>          // std::mutex, std::lock_guard
#include <utility>        // std::pair
#include <streambuf>      // std::streambuf
#include <istream>        // std::basic_istream
#include <memory>         // std::unique_ptr
#include "boost/crc.hpp"  // Boost.CRC library

//-------------------------------------------------------------------------------------------------
namespace DSrv
{

//-------------------------------------------------------------------------------------------------
class Storage_2_buffers
{

	friend class Storage_2_buffers_test;

public:
	
	// Data type for set method (pointer + size)
	typedef  const std::pair<const uint8_t * const, const uint32_t>  Data_set;
	
	// Data type for get method (pointer + size)
	typedef  std::pair<const uint8_t *, uint32_t>  Data_get;

	// Constructor
	Storage_2_buffers();
	
	// Destructor
	virtual ~Storage_2_buffers();
	
	// Copy constructor
	Storage_2_buffers(const Storage_2_buffers & obj);

	// Move constructor
	Storage_2_buffers(Storage_2_buffers && obj);
	
	// Override an assignment operator
	Storage_2_buffers & operator=(const Storage_2_buffers & obj);
	
	// Allocates memory for buffers
	int32_t allocate(const uint32_t size) noexcept;
	
	// Deallocates memory for buffers
	int32_t deallocate() noexcept;

	// Sets data in the storage
	int32_t setData(Data_set data) noexcept;

	// Gets complete data from the storage
	int32_t getData(Data_get & data) noexcept;

	// Deletes all data in the storage
	int32_t clearData() noexcept;

	// Sets filling data as complete
	int32_t completeData() noexcept;
	
	// Gets a CRC of complete data
	uint32_t completeDataCRC();
	
	// Gets pointer to istream object
	std::istream * getIstreamPointer() noexcept
	{
		return &m_istream;
	}
	
	// Applies clear() method to the std::istream object
	void clearIstream() noexcept;

	// Enables debug messages
	void setDebug(const bool d) noexcept
	{
		m_debug = d;
	}

private:

	// Allocates memory for buffers without lock a mutex
	int32_t allocate_(const uint32_t size) noexcept;
	
	// Deallocates memory for buffers without lock a mutex
	void deallocate_() noexcept;

	// Buffer class
	class Streambuf : public std::streambuf
	{
	
	public:
		
		// Default constructor
		Streambuf() = default;
	
		// Move constructor
		Streambuf(Streambuf && obj);
		
		// Sets the pointers 
		void setPointers(char * begin, char * curr, char * end)
		{
			setg(begin, curr, end);
		}
		
	} m_streambuf;
	
	// Input stream
	std::istream m_istream;

	// Pointers to the data
	std::unique_ptr<uint8_t []> m_completeData {nullptr};
	std::unique_ptr<uint8_t []> m_fillingData {nullptr};
	
	// Data size
	const uint32_t m_dataSize {0};

	// Index in the array of filling data
	uint32_t m_fillingIndex {0};

	// Size of the complete data
	uint32_t m_completeSize {0};

	// Mutex
	std::mutex m_mutex;
	
	// CRC
	boost::crc_32_type m_crc;

	// Enable debug messages
	bool m_debug {true};
};

//-------------------------------------------------------------------------------------------------
} // namespace DSrv
