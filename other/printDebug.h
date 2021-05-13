
#pragma once

//-------------------------------------------------------------------------------------------------
/*
DESCRITION: macroses and function for print debug messages
TODO:
FIXME:
DANGER:
NOTE:
 * __FUNCTION__ is a __func__ synonym

Thread safety: yes
Reentrance: yes
*/

//-------------------------------------------------------------------------------------------------
#include <iostream>  // std::cout, std::endl

//-------------------------------------------------------------------------------------------------
namespace DSrv
{

//-------------------------------------------------------------------------------------------------
#define PRINT_DBG(enable, ...)                                   \
	do {                                                         \
	    if (enable)                                              \
	        DSrv::printDebug(__FUNCTION__, false, __VA_ARGS__);  \
	} while(0)

//-------------------------------------------------------------------------------------------------
#define PRINT_ERR(...)                                      \
	do {                                                    \
	    DSrv::printDebug(__FUNCTION__, true, __VA_ARGS__);  \
	} while(0)

//-------------------------------------------------------------------------------------------------
void printDebug(const char* func,
                const bool err,
                const char* format,
                ...) noexcept;

//-------------------------------------------------------------------------------------------------
} // namespace DSrv
