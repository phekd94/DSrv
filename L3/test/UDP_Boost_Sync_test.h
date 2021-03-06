
#pragma once

//-------------------------------------------------------------------------------------------------
/*
DESCRITION: test class for L3/UDP_Boost_Sync class
TODO:
FIXME:
DANGER:
NOTE:
*/

//-------------------------------------------------------------------------------------------------
#include "DSrv/L3/UDP_Boost_Sync.h"

//-------------------------------------------------------------------------------------------------
namespace DSrv {

//-------------------------------------------------------------------------------------------------
template <typename Storage, template <typename T> class Base>
class UDP_Boost_Sync_for_test : public UDP_Boost_Sync<Storage, Base>
{
	virtual int32_t dataParser(typename Base<Storage>::Data_parser data) noexcept override final
	{
		return 0;
	}
};

//=================================================================================================
template <typename Storage, template <typename T> class Base>
class UDP_Boost_Sync_test
{
public:

	UDP_Boost_Sync_test() = delete;
	
	// Tests a move constructor
	static int32_t move() noexcept;
	
	// Tests a copy constructor
	static int32_t copy() noexcept;
	
	// Tests an operator=
	static int32_t opEqual() noexcept;
	
	// Runs all tests
	static int32_t fullTest() noexcept;
};

//-------------------------------------------------------------------------------------------------
template <typename Storage, template <typename T> class Base>
int32_t UDP_Boost_Sync_test<Storage, Base>::move() noexcept
{
	PRINT_DBG(true, "------ move ------");

	UDP_Boost_Sync_for_test<Storage, Base> obj_1;
	obj_1.allocate(95);
	obj_1.start(50000);

	// Apply move constructor
	{
		UDP_Boost_Sync_for_test<Storage, Base> obj_2 {std::move(obj_1)};
		
		if (obj_2.receiveData() < 0)
		{
			PRINT_ERR("obj_2.receiveData()");
			return -1;
		}
	}
	
	if (!(obj_1.receiveData() < 0))
	{
		PRINT_ERR("obj_1.receiveData()");
		return -1;
	}

	return 0;
}

//-------------------------------------------------------------------------------------------------
template <typename Storage, template <typename T> class Base>
int32_t UDP_Boost_Sync_test<Storage, Base>::copy() noexcept
{
	PRINT_DBG(true, "------ copy ------");

	UDP_Boost_Sync_for_test<Storage, Base> obj_1;
	obj_1.allocate(95);
	obj_1.start(50000);

	// Apply copy constructor
	{
		UDP_Boost_Sync_for_test<Storage, Base> obj_2 {obj_1};
		
		if (!(obj_2.receiveData() < 0))
		{
			PRINT_ERR("obj_2.receiveData()");
			return -1;
		}
	}
	
	if (obj_1.receiveData() < 0)
	{
		PRINT_ERR("obj_1.receiveData()");
		return -1;
	}

	return 0;
}

//-------------------------------------------------------------------------------------------------
template <typename Storage, template <typename T> class Base>
int32_t UDP_Boost_Sync_test<Storage, Base>::opEqual() noexcept
{
	PRINT_DBG(true, "------ opEqual ------");
	
	UDP_Boost_Sync_for_test<Storage, Base> obj_1;
	obj_1.allocate(10);
	obj_1.start(50000);
	UDP_Boost_Sync_for_test<Storage, Base> obj_2;
	obj_2.allocate(3);
	obj_2.start(50001);

	// Apply operator=
	obj_1 = obj_1;
	obj_2 = obj_1;
	
	return 0;
}

//-------------------------------------------------------------------------------------------------
template <typename Storage, template <typename T> class Base>
int32_t UDP_Boost_Sync_test<Storage, Base>::fullTest() noexcept
{
	PRINT_DBG(true, "====== fullTest ======");

	if (move() != 0)
	{
		PRINT_ERR("move");
		return -1;
	}
	
	if (copy() != 0)
	{
		PRINT_ERR("move");
		return -1;
	}
	
	if (opEqual() != 0)
	{
		PRINT_ERR("opEqual");
		return -1;
	}

	PRINT_DBG(true, "====== Test was successful ======");
	return 0;
}

//-------------------------------------------------------------------------------------------------
} // namespace DSrv
