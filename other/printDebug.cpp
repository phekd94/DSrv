
#include "printDebug.h"

#include <cstdio>     // std::vsprintf(), std::sprintf()
#include <cstdarg>    // va_list macro, std::va_start(), va_end macro
#include <cstdint>    // integer types
#include <mutex>      // std::mutex, std::lock_guard
#include <exception>  // std::exception

//-------------------------------------------------------------------------------------------------
bool CHECK_PRINTF_ERROR(const int ret,
                        const char* const func, 
                        const char* const format) noexcept
{
	if (ret < 0) 
	{
		const uint32_t BUF_SIZE = 500;
		char out[BUF_SIZE];
		if (std::sprintf(out, "%s(): %s(): "
		                      "format encoding error occurred; format: \n\t\"%s\"",
		                      func, __FUNCTION__, format) < 0)
		{
			std::cout << __FUNCTION__ << ": sprintf() error" << std::endl;
			return false;
		}
		std::cout << out << std::endl;
		return false;
	} 
	else 
	{
		return true;
	}
}

//-------------------------------------------------------------------------------------------------
void DSrv::printDebug(const char* func,
                      const bool err, 
                      const char* format, 
                      ...) noexcept
{
	const uint32_t BUF_SIZE = 500;

	char buf[BUF_SIZE], userMsg[BUF_SIZE], out[BUF_SIZE + BUF_SIZE];
	va_list argList;
	int ret;

	// printDebug() for all threads
	static std::mutex mutex;

	if (nullptr == func || nullptr == format)
	{
		if (nullptr == func)
			func = "nullptr ";
		if (nullptr == format)
			format = "nullptr";
		CHECK_PRINTF_ERROR(-1, func, format);
		return;
	}

	va_start(argList, format);
	ret = std::vsprintf(userMsg, format, argList);
	va_end(argList);
	if (CHECK_PRINTF_ERROR(ret, func, format) == false)
		return;

	ret = std::sprintf(buf, "%s%s(): ", err ? "ERROR: " : "", func);
	if (CHECK_PRINTF_ERROR(ret, func, format) == false)
		return;

	ret = std::sprintf(out, "%s%s", buf, userMsg);
	if (CHECK_PRINTF_ERROR(ret, func, format) == false)
		return;

	try {
		std::lock_guard<std::mutex> lock(mutex);

		std::cout << out << std::endl;
	}
	catch (std::exception& obj)
	{
		std::cout << func << "(): " << __FUNCTION__ << "(): "
		          << "Exception from mutex.lock() or from Qt functions has been occured: "
		          << obj.what() << std::endl;
	}
}
