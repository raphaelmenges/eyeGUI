#include "helpers-errors.hpp"

namespace helpers {

	std::string error(int32_t error)
	{
	#define ERROR_CASE(_name) case UTF8_ERR_ ## _name: return "UTF8_ERR_" # _name;

		switch (error)
		{

		ERROR_CASE(NONE);
		ERROR_CASE(INVALID_DATA);
		ERROR_CASE(INVALID_FLAG);
		ERROR_CASE(NOT_ENOUGH_SPACE);
		ERROR_CASE(OVERLAPPING_PARAMETERS);

		default:
			std::stringstream ss;
			ss << "<invalid> (0x";
			ss << std::hex << std::setfill('0') << std::setw(2) << std::uppercase << error;
			ss << ")";
			return ss.str();

		}

	#undef ERROR_CASE
	}

	::testing::AssertionResult CompareErrors(
		const char* expressionExpected GTEST_ATTRIBUTE_UNUSED_, const char* expressionActual GTEST_ATTRIBUTE_UNUSED_,
		int32_t errorExpected, int32_t errorActual)
	{
		if (errorExpected == errorActual)
		{
			return ::testing::AssertionSuccess();
		}
		else
		{
			::testing::AssertionResult result = ::testing::AssertionFailure();

			result << std::endl;

			result << "  Actual: " << error(errorActual) << " (" << errorActual << ")" << std::endl;
			result << "Expected: " << error(errorExpected) << " (" << errorExpected << ")";

			return result;
		}
	}

};