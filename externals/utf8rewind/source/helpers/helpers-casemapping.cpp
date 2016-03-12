#include "helpers-casemapping.hpp"

#include "helpers-strings.hpp"

extern "C" {
	#include "../internal/casemapping.h"
}

namespace helpers {

	std::string uppercase(unicode_t codepoint)
	{
		return uppercase(utf8(codepoint));
	}

	std::string uppercase(const std::string& text)
	{
		std::string converted;
		int32_t errors;

		size_t size_in_bytes = utf8toupper(text.c_str(), text.length(), nullptr, 0, &errors);
		if (size_in_bytes == 0 ||
			errors != UTF8_ERR_NONE)
		{
			return converted;
		}

		converted.resize(size_in_bytes);
		utf8toupper(text.c_str(), text.length(), &converted[0], size_in_bytes, nullptr);

		return converted;
	}

	std::string lowercase(unicode_t codepoint)
	{
		return lowercase(utf8(codepoint));
	}

	std::string lowercase(const std::string& text)
	{
		std::string converted;
		int32_t errors;

		size_t size_in_bytes = utf8tolower(text.c_str(), text.length(), nullptr, 0, &errors);
		if (size_in_bytes == 0 ||
			errors != UTF8_ERR_NONE)
		{
			return converted;
		}

		converted.resize(size_in_bytes);
		utf8tolower(text.c_str(), text.length(), &converted[0], size_in_bytes, nullptr);

		return converted;
	}

	std::string titlecase(unicode_t codepoint)
	{
		return titlecase(utf8(codepoint));
	}

	std::string titlecase(const std::string& text)
	{
		std::string converted;
		int32_t errors;

		size_t size_in_bytes = utf8totitle(text.c_str(), text.length(), nullptr, 0, &errors);
		if (size_in_bytes == 0 ||
			errors != UTF8_ERR_NONE)
		{
			return converted;
		}

		converted.resize(size_in_bytes);
		utf8totitle(text.c_str(), text.length(), &converted[0], size_in_bytes, nullptr);

		return converted;
	}

#if UTF8_VERSION_GUARD(1, 4, 0)
	std::string casefold(unicode_t codepoint)
	{
		return casefold(utf8(codepoint));
	}

	std::string casefold(const std::string& text)
	{
		std::string converted;
		int32_t errors;

		size_t size_in_bytes = utf8casefold(text.c_str(), text.length(), nullptr, 0, &errors);
		if (size_in_bytes == 0 ||
			errors != UTF8_ERR_NONE)
		{
			return converted;
		}

		converted.resize(size_in_bytes);
		utf8casefold(text.c_str(), text.length(), &converted[0], size_in_bytes, nullptr);

		return converted;
	}
#endif

	::testing::AssertionResult CompareCodepoint(
		const char* expressionExpected GTEST_ATTRIBUTE_UNUSED_, const char* expressionActual GTEST_ATTRIBUTE_UNUSED_,
		const CaseMappingEntry& entryExpected, const CaseMappingEntry& entryActual)
	{
		if (entryExpected.lowercase == entryActual.lowercase &&
			entryExpected.uppercase == entryActual.uppercase &&
			entryExpected.titlecase == entryActual.titlecase)
		{
			return ::testing::AssertionSuccess();
		}
		else
		{
			::testing::AssertionResult result = ::testing::AssertionFailure();

			result << entryExpected.name << " (" << identifiable(entryExpected.codepoint)  << ")" << std::endl;

			result << std::endl;

			if (entryExpected.lowercase != entryActual.lowercase)
			{
				result << "[Lowercase]" << std::endl;
				result << "    Actual:  \"" << printable(entryActual.lowercase) << "\" (" << identifiable(entryActual.lowercase) << ")" << std::endl;
				result << "  Expected:  \"" << printable(entryExpected.lowercase) << "\" (" << identifiable(entryExpected.lowercase) << ")" << std::endl;
			}
			else
			{
				result << "[Lowercase]  \"" << printable(entryExpected.lowercase) << "\" (" << identifiable(entryExpected.lowercase) << ")" << std::endl;
			}

			result << std::endl;

			if (entryExpected.uppercase != entryActual.uppercase)
			{
				result << "[Uppercase]" << std::endl;
				result << "    Actual:  \"" << printable(entryActual.uppercase) << "\" (" << identifiable(entryActual.uppercase) << ")" << std::endl;
				result << "  Expected:  \"" << printable(entryExpected.uppercase) << "\" (" << identifiable(entryExpected.uppercase) << ")" << std::endl;
			}
			else
			{
				result << "[Uppercase]  \"" << printable(entryActual.uppercase) << "\" (" << identifiable(entryActual.uppercase) << ")" << std::endl;
			}

			result << std::endl;

			if (entryExpected.titlecase != entryActual.titlecase)
			{
				result << "[Titlecase]" << std::endl;
				result << "    Actual:  \"" << printable(entryActual.titlecase) << "\" (" << identifiable(entryActual.titlecase) << ")" << std::endl;
				result << "  Expected:  \"" << printable(entryExpected.titlecase) << "\" (" << identifiable(entryExpected.titlecase) << ")" << std::endl;
			}
			else
			{
				result << "[Titlecase]  \"" << printable(entryExpected.titlecase) << "\" (" << identifiable(entryExpected.titlecase) << ")" << std::endl;
			}

			return result;
		}
	}

	::testing::AssertionResult CompareCaseMapping(
		const char* expressionExpected GTEST_ATTRIBUTE_UNUSED_, const char* expressionActual GTEST_ATTRIBUTE_UNUSED_,
		const CaseMappingEntry& entryExpected, const CaseMappingEntry& entryActual)
	{
		if (entryExpected.lowercase == entryActual.lowercase &&
			entryExpected.uppercase == entryActual.uppercase &&
			entryExpected.titlecase == entryActual.titlecase)
		{
			return ::testing::AssertionSuccess();
		}
		else
		{
			::testing::AssertionResult result = ::testing::AssertionFailure();

			result << "Input: \"" << printable(entryActual.input) << "\" (" << identifiable(entryActual.input) << ")" << std::endl;

			result << std::endl;

			if (entryExpected.lowercase != entryActual.lowercase)
			{
				result << "[Lowercase]" << std::endl;
				result << "    Actual:  \"" << printable(entryActual.lowercase) << "\" (" << identifiable(entryActual.lowercase) << ")" << std::endl;
				result << "  Expected:  \"" << printable(entryExpected.lowercase) << "\" (" << identifiable(entryExpected.lowercase) << ")" << std::endl;
			}
			else
			{
				result << "[Lowercase]  \"" << printable(entryExpected.lowercase) << "\" (" << identifiable(entryExpected.lowercase) << ")" << std::endl;
			}

			result << std::endl;

			if (entryExpected.uppercase != entryActual.uppercase)
			{
				result << "[Uppercase]" << std::endl;
				result << "    Actual:  \"" << printable(entryActual.uppercase) << "\" (" << identifiable(entryActual.uppercase) << ")" << std::endl;
				result << "  Expected:  \"" << printable(entryExpected.uppercase) << "\" (" << identifiable(entryExpected.uppercase) << ")" << std::endl;
			}
			else
			{
				result << "[Uppercase]  \"" << printable(entryActual.uppercase) << "\" (" << identifiable(entryActual.uppercase) << ")" << std::endl;
			}

			result << std::endl;

			if (entryExpected.titlecase != entryActual.titlecase)
			{
				result << "[Titlecase]" << std::endl;
				result << "    Actual:  \"" << printable(entryActual.titlecase) << "\" (" << identifiable(entryActual.titlecase) << ")" << std::endl;
				result << "  Expected:  \"" << printable(entryExpected.titlecase) << "\" (" << identifiable(entryExpected.titlecase) << ")" << std::endl;
			}
			else
			{
				result << "[Titlecase]  \"" << printable(entryExpected.titlecase) << "\" (" << identifiable(entryExpected.titlecase) << ")" << std::endl;
			}

			return result;
		}
	}

#if UTF8_VERSION_GUARD(1, 4, 0)
	::testing::AssertionResult CompareCaseFolding(
		const char* expressionExpected GTEST_ATTRIBUTE_UNUSED_, const char* expressionActual GTEST_ATTRIBUTE_UNUSED_,
		const CaseFoldingEntry& entryExpected, const CaseFoldingEntry& entryActual)
	{
		if (entryExpected.folded == entryActual.folded)
		{
			return ::testing::AssertionSuccess();
		}
		else
		{
			::testing::AssertionResult result = ::testing::AssertionFailure();

			result << entryExpected.name << " (" << identifiable(entryExpected.codePoint)  << ")" << std::endl;

			result << std::endl;

			result << "    Actual:  \"" << printable(entryActual.folded) << "\" (" << identifiable(entryActual.folded) << ")" << std::endl;
			result << "  Expected:  \"" << printable(entryExpected.folded) << "\" (" << identifiable(entryExpected.folded) << ")" << std::endl;

			return result;
		}
	}
#endif

};