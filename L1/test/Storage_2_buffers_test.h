
#pragma once

//-------------------------------------------------------------------------------------------------
/*
DESCRITION: test class for Storage_2_buffers class
TODO:
 * test for move constructor
 * test for mutex
FIXME:
DANGER:
NOTE:
*/

//-------------------------------------------------------------------------------------------------
#include "DSrv/L1/Storage_2_buffers.h"

//-------------------------------------------------------------------------------------------------
namespace DSrv
{

//-------------------------------------------------------------------------------------------------
class Storage_2_buffers_test
{
public:

	// Only via public static methods
	Storage_2_buffers_test() = delete;

	// Tests methods which utilize pointers
	static int32_t pNull(Storage_2_buffers & obj) noexcept;

	// Tests a work with data
	static int32_t data(Storage_2_buffers & obj) noexcept;

	// Tests a move constructor
	// static int32_t move() noexcept;
	
	// Test a CRC
	static int32_t crc(Storage_2_buffers & obj) noexcept;

	// Runs all tests
	static int32_t fullTest() noexcept;
};

//-------------------------------------------------------------------------------------------------
} // namespace DSrv
