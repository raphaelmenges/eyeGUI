#include "helpers-locale.hpp"

extern "C" {
	#include "../internal/casemapping.h"
}

namespace helpers {

#if UTF8_VERSION_GUARD(1, 3, 0)
	std::string locale(uint32_t value)
	{
	#define LOCALE_CASE(_name) case CASEMAPPING_LOCALE_ ## _name: return "CASEMAPPING_LOCALE_" # _name;

		switch (value)
		{

		LOCALE_CASE(DEFAULT);
		LOCALE_CASE(LITHUANIAN);
		LOCALE_CASE(TURKISH_OR_AZERI_LATIN);
		
		default:
			std::stringstream ss;
			ss << "<invalid> (0x";
			ss << std::hex << std::setfill('0') << std::setw(2) << std::uppercase << value;
			ss << ")";
			return ss.str();

		}

	#undef LOCALE_CASE
	}
#else
	std::string locale(uint32_t value)
	{
		return "";
	}
#endif

	::testing::AssertionResult CompareLocale(
		const char* expressionExpected GTEST_ATTRIBUTE_UNUSED_, const char* expressionActual,
		uint32_t localeExpected, uint32_t localeActual)
	{
		if (localeExpected == localeActual)
		{
			return ::testing::AssertionSuccess();
		}
		else
		{
			::testing::AssertionResult result = ::testing::AssertionFailure();

			result << "Value of: " << expressionActual << std::endl;

			result << "  Actual: " << locale(localeActual) << " (" << localeActual << ")" << std::endl;
			result << "Expected: " << locale(localeExpected) << " (" << localeExpected << ")";

			return result;
		}
	}

};