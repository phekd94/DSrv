
#include "Storage_2_buffers_test.h"

#include <utility>    // std::move
#include <exception>  // std::exception

#include "DSrv/other/printDebug.h"  // PRINT_DBG, PRINT_ERR

//-------------------------------------------------------------------------------------------------
using namespace DSrv;

//-------------------------------------------------------------------------------------------------
const uint32_t maxSize = 2000;

//-------------------------------------------------------------------------------------------------
int32_t Storage_2_buffers_test::pNull(Storage_2_buffers & obj) noexcept
{
	Storage_2_buffers::Data_set dataNull(nullptr, 0);
	Storage_2_buffers::Data_set dataSet;
	Storage_2_buffers::Data_get dataGet;

	// Test all methods which utilize input pointers
	if (obj.setData(dataNull) == 0)
	{
		PRINT_ERR("setData(dataNull)");
		return -1;
	}

	// Test all methods which utilize data pointers
	std::unique_ptr<uint8_t []> m_completeData {nullptr};
	std::unique_ptr<uint8_t []> m_fillingData {nullptr};
	std::swap(m_completeData, obj.m_completeData);
	std::swap(m_fillingData, obj.m_fillingData);

	if (obj.setData(dataSet) == 0)
	{
		PRINT_ERR("setData() with nullptr == m_...Data");
		return -1;
	}
	if (obj.getData(dataGet) == 0)
	{
		PRINT_ERR("getData() with nullptr == m_...Data");
		return -1;
	}
	if (obj.completeData() == 0)
	{
		PRINT_ERR("completeData() with nullptr == m_...Data");
		return -1;
	}

	std::swap(m_completeData, obj.m_completeData);
	std::swap(m_fillingData, obj.m_fillingData);

	return 0;
}

//-------------------------------------------------------------------------------------------------
int32_t Storage_2_buffers_test::data(Storage_2_buffers & obj) noexcept
{
	uint8_t data_1[] {1};
	uint8_t data_2[] {2};
	Storage_2_buffers::Data_set dataSet_1(data_1, 1);
	Storage_2_buffers::Data_set dataSet_2(data_2, 1);
	Storage_2_buffers::Data_get dataGet;
	Storage_2_buffers::Data_set dataSet_max(data_1, maxSize);
	Storage_2_buffers::Data_set dataSet_maxP1(data_2, maxSize + 1);

	// Get if data is not complete
	if (obj.setData(dataSet_1) != 0)
	{
		PRINT_ERR("setData(data_1)");
		return -1;
	}
	if (obj.getData(dataGet) != 0)
	{
		PRINT_ERR("getData()");
		return -1;
	}
	else
	{
		if (dataGet.second != 0)
		{
			PRINT_ERR("size_r after first setData()");
			return -1;
		}
	}
	if (obj.completeData() != 0)
	{
		PRINT_ERR("completeData() after first setData()");
		return -1;
	}
	if (obj.getData(dataGet) != 0)
	{
		PRINT_ERR("getData() after first completeData()");
		return -1;
	}
	else
	{
		if (dataGet.second != 1 || dataGet.first[0] != 1)
		{
			PRINT_ERR("size_r or data_r after first completeData()");
			return -1;
		}
	}

	// Two setData
	if (obj.setData(dataSet_1) != 0)
	{
		PRINT_ERR("setData(data_2)");
		return -1;
	}
	if (obj.setData(dataSet_2) != 0)
	{
		PRINT_ERR("setData(data_1)");
		return -1;
	}
	if (obj.completeData() != 0)
	{
		PRINT_ERR("completeData() for two setData()");
		return -1;
	}
	if (obj.getData(dataGet) != 0)
	{
		PRINT_ERR("getData() for two setData()");
		return -1;
	}
	else
	{
		if (dataGet.second != 2 || dataGet.first[0] != 1 || dataGet.first[1] != 2)
		{
			PRINT_ERR("size_r or data_r for two setData()");
			return -1;
		}
	}

	// maxSize + 1 (new data)
	if (obj.setData(dataSet_maxP1) == 0)
	{
		PRINT_ERR("setData(dataSet_maxP1)");
		return -1;
	}

	// maxSize (add data)
	if (obj.setData(dataSet_1) != 0)
	{
		PRINT_ERR("setData(dataSet_1)");
		return -1;
	}
	if (obj.setData(dataSet_max) == 0)
	{
		PRINT_ERR("setData(dataSet_max)");
		return -1;
	}

	// Clear method
	if (obj.clearData() != 0)
	{
		PRINT_ERR("clearData()");
		return -1;
	}
	if (obj.getData(dataGet) != 0)
	{
		PRINT_ERR("getData()");
		return -1;
	}
	else
	{
		if (dataGet.second != 0)
		{
			PRINT_ERR("size_r after clearData()");
			return -1;
		}
	}

	return 0;
}

//-------------------------------------------------------------------------------------------------
/*
int32_t Storage_2_buffers_test::move() noexcept
{
	Storage_2_buffers obj_1;

	// Save values of pointers
	const auto m_completeData {obj_1.m_completeData};
	const auto m_fillingData {obj_1.m_fillingData};

	// Apply move constructor
	Storage_2_buffers obj_2 {std::move(obj_1)};

	// Check obj_1 pointers
	if (obj_1.m_completeData != nullptr || obj_1.m_fillingData != nullptr)
	{
		PRINT_ERR("m_completeData or m_fillingData of obj_1 is not equal nullptr");
		return -1;
	}

	// Check obj_2 pointers
	if (obj_2.m_completeData != m_completeData || obj_2.m_fillingData != m_fillingData)
	{
		PRINT_ERR("m_completeData or m_fillingData of obj_1 is not equal saved pointers");
		return -1;
	}

	return 0;
}
*/

//-------------------------------------------------------------------------------------------------
int32_t Storage_2_buffers_test::crc(Storage_2_buffers & obj) noexcept
{
	uint8_t data[1] = {1};
	uint32_t size = 1;

	if (obj.setData(Storage_2_buffers::Data_set(data, size)) != 0)
	{
		PRINT_ERR("setData(data_1)");
		return -1;
	}
	if (obj.completeData() != 0)
	{
		PRINT_ERR("completeData() after first setData()");
		return -1;
	}
	
	try {
		if (obj.completeDataCRC() != 2768625435u)
		{
			PRINT_ERR("completeDataCRC() is not correct");
			return -1;
		}
	}
	catch (std::exception & ex)
	{
		PRINT_ERR("%s", ex.what());
	}
	
	if (obj.clearData() != 0)
	{
		PRINT_ERR("clearData()");
		return -1;
	}
	
	return 0;
}

//-------------------------------------------------------------------------------------------------
int32_t Storage_2_buffers_test::fullTest() noexcept
{
	Storage_2_buffers obj(maxSize);
	obj.setDebug(true);

	if (pNull(obj) != 0)
	{
		PRINT_ERR("pNull");
		return -1;
	}

	if (data(obj) != 0)
	{
		PRINT_ERR("data");
		return -1;
	}
	
	if (crc(obj) != 0)
	{
		PRINT_ERR("crc");
		return -1;
	}

	/*
	if (move() != 0)
	{
		PRINT_ERR("move");
		return -1;
	}
	*/

	PRINT_DBG(true, "Test was successful");
	return 0;
}
