
#pragma once

//-------------------------------------------------------------------------------------------------
/*
DESCRITION: test class for L2/Base class
TODO:
FIXME:
DANGER:
NOTE:
*/

//-------------------------------------------------------------------------------------------------
#include "DSrv/L2/Base.h"  // Base class

//-------------------------------------------------------------------------------------------------
namespace DSrv
{

//-------------------------------------------------------------------------------------------------
// Class for test a Base class (with override methods)
template <typename Storage>
class Base_for_test : public Base<Storage>
{

public:

	explicit Base_for_test(const uint32_t bufferSize) : Base<Storage>(bufferSize)
	{
	}

private:
	
	virtual int32_t sendData(const typename Base<Storage>::Data_send) noexcept override final
	{
		return 0;
	}
	
	virtual int32_t receiveData() noexcept override final
	{
		return 0;
	}

	virtual int32_t dataParser(typename Base<Storage>::Data_parser) noexcept override final
	{
		return 0;
	}
};

//=================================================================================================
template <typename Storage>
class Base_test
{

public:

	// Only via public static methods
	Base_test() = delete;

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
template <typename Storage>
int32_t Base_test<Storage>::move() noexcept
{
	PRINT_DBG(true, "------ move ------");

	Base_for_test<Storage> obj_1 {10};

	// Apply move constructor
	Base_for_test<Storage> obj_2 {std::move(obj_1)};
	
	return 0;
}

//-------------------------------------------------------------------------------------------------
template <typename Storage>
int32_t Base_test<Storage>::copy() noexcept
{
	PRINT_DBG(true, "------ copy ------");

	Base_for_test<Storage> obj_1 {10};

	// Apply copy constructor
	Base_for_test<Storage> obj_2 {obj_1};
	
	return 0;
}

//-------------------------------------------------------------------------------------------------
template <typename Storage>
int32_t Base_test<Storage>::opEqual() noexcept
{
	PRINT_DBG(true, "------ opEqual ------");

	Base_for_test<Storage> obj_1 {10};
	Base_for_test<Storage> obj_2 {3};

	// Apply operator=
	obj_1 = obj_1;
	obj_2 = obj_1;
	
	return 0;
}

//-------------------------------------------------------------------------------------------------
template <typename Storage>
int32_t Base_test<Storage>::fullTest() noexcept
{
	PRINT_DBG(true, "====== fullTest ======");
	
	if (move() != 0)
	{
		PRINT_ERR("move");
		return -1;
	}
	
	if (copy() != 0)
	{
		PRINT_ERR("copy");
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
