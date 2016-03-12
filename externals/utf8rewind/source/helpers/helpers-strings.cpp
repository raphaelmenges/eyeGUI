#include "helpers-strings.hpp"

extern "C" {
	#include "../internal/database.h"
};

namespace helpers {

	void identifiable(std::stringstream& target, unicode_t codepoint)
	{
		target << "U+" << std::hex << std::setfill('0') << std::setw(4) << std::uppercase << codepoint;
	}

	std::string identifiable(unicode_t codepoint)
	{
		std::stringstream ss;
		identifiable(ss, codepoint);
		return ss.str();
	}

	std::string identifiable(unicode_t* codepoint, size_t codepointsSize)
	{
		std::stringstream ss;

		identifiable(ss, codepoint[0]);

		for (size_t i = 1; i < codepointsSize / sizeof(unicode_t); ++i)
		{
			ss << " ";
			identifiable(ss, codepoint[i]);
		}

		return ss.str();
	}

	std::string identifiable(const std::string& text)
	{
		std::vector<unicode_t> converted = utf32(text);
		if (converted.size() == 0)
		{
			return "";
		}

		std::stringstream ss;

		for (std::vector<unicode_t>::iterator it = converted.begin(); it != converted.end(); ++it)
		{
			if (it != converted.begin())
			{
				ss << " ";
			}
			identifiable(ss, *it);
		}

		return ss.str();
	}

	std::string utf8(unicode_t codepoint)
	{
		std::string converted;

		int32_t errors;
		size_t size_in_bytes = utf32toutf8(
			&codepoint, sizeof(unicode_t),
			nullptr, 0,
			&errors);

		if (size_in_bytes == 0 ||
			errors != UTF8_ERR_NONE)
		{
			return converted;
		}

		converted.resize(size_in_bytes);

		utf32toutf8(
			&codepoint, sizeof(unicode_t),
			&converted[0], size_in_bytes,
			nullptr);

		return converted;
	}

	std::string utf8(unicode_t* codepoints, size_t codepointsSize)
	{
		std::string converted;

		int32_t errors;
		size_t size_in_bytes = utf32toutf8(
			codepoints, codepointsSize,
			nullptr, 0,
			&errors);

		if (size_in_bytes == 0 ||
			errors != UTF8_ERR_NONE)
		{
			return converted;
		}

		converted.resize(size_in_bytes);

		utf32toutf8(
			codepoints, codepointsSize,
			&converted[0], size_in_bytes,
			nullptr);

		return converted;
	}

	std::string utf8(const std::vector<unicode_t>& codepoints)
	{
		std::string converted;

		int32_t errors;
		size_t size_in_bytes = utf32toutf8(
			&codepoints[0], codepoints.size() * sizeof(unicode_t),
			nullptr, 0,
			&errors);

		if (size_in_bytes == 0 ||
			errors != UTF8_ERR_NONE)
		{
			return converted;
		}

		converted.resize(size_in_bytes);

		utf32toutf8(
			&codepoints[0], codepoints.size() * sizeof(unicode_t),
			&converted[0], size_in_bytes,
			nullptr);

		return converted;
	}

	std::string utf8(const std::wstring& text)
	{
		std::string converted;

		int32_t errors;
		size_t size_in_bytes = widetoutf8(
			text.c_str(), text.size(),
			nullptr, 0,
			&errors);

		if (size_in_bytes == 0 ||
			errors != UTF8_ERR_NONE)
		{
			return converted;
		}

		converted.resize(size_in_bytes);

		widetoutf8(
			text.c_str(), text.size(),
			&converted[0], size_in_bytes,
			&errors);

		return converted;
	}

	std::vector<utf16_t> utf16(const std::string& text)
	{
		std::vector<utf16_t> converted;

		int32_t errors;
		size_t size_in_bytes = utf8toutf16(
			text.c_str(), text.size(),
			nullptr, 0,
			&errors);

		if (size_in_bytes == 0 ||
			errors != UTF8_ERR_NONE)
		{
			return converted;
		}

		converted.resize(size_in_bytes / sizeof(utf16_t));

		utf8toutf16(
			text.c_str(), text.size(),
			&converted[0], size_in_bytes,
			&errors);

		return converted;
	}

	std::vector<unicode_t> utf32(const std::string& text)
	{
		std::vector<unicode_t> converted;

		int32_t errors;
		size_t size_in_bytes = utf8toutf32(
			text.c_str(), text.size(),
			nullptr, 0,
			&errors);

		if (size_in_bytes == 0 ||
			errors != UTF8_ERR_NONE)
		{
			return converted;
		}

		converted.resize(size_in_bytes / sizeof(unicode_t));

		utf8toutf32(
			text.c_str(), text.size(),
			&converted[0], size_in_bytes,
			&errors);

		return converted;
	}

	std::wstring wide(const std::string& text)
	{
		std::wstring converted;

		int32_t errors;
		size_t size_in_bytes = utf8towide(
			text.c_str(), text.size(),
			nullptr, 0,
			&errors);

		if (size_in_bytes == 0 ||
			errors != UTF8_ERR_NONE)
		{
			return converted;
		}

		converted.resize(size_in_bytes / UTF8_WCHAR_SIZE);

		utf8towide(
			text.c_str(), text.size(),
			&converted[0], size_in_bytes,
			&errors);

		return converted;
	}

	void printable(std::stringstream& target, bool& wroteHex, uint8_t character)
	{
		if (character < 0x20)
		{
			wroteHex = false;

			switch (character)
			{

			case 0:
				break;

			case '\a':
				target << "\\a";
				break;

			case '\b':
				target << "\\b";
				break;

			case '\f':
				target << "\\f";
				break;

			case '\n':
				target << "\\n";
				break;

			case '\r':
				target << "\\r";
				break;

			case '\t':
				target << "\\t";
				break;

			case '\v':
				target << "\\v";
				break;

			default:
				target << "\\x" << std::uppercase << std::setfill('0') << std::hex << std::setw(2) << ((unicode_t)character & 0x000000FF);
				wroteHex = true;
				break;

			}
		}
		else if (character <= 0x7F)
		{
			if (wroteHex)
			{
				if ((character >= 'A' && character <= 'F') ||
					(character >= 'a' && character <= 'f') ||
					(character >= '0' && character <= '9'))
				{
					char head = *(--target.str().end());

					if ((head >= 'A' && head <= 'F') ||
						(head >= 'a' && head <= 'f') ||
						(head >= '0' && head <= '9'))
					{
						target << "\" \"";
					}
				}
			}

			target.put(character);

			wroteHex = false;
		}
		else
		{
			target << "\\x" << std::uppercase << std::setfill('0') << std::hex << std::setw(2) << ((unicode_t)character & 0x000000FF);

			wroteHex = true;
		}
	}

	std::string hex(unicode_t codepoint)
	{
		return hex(utf8(codepoint));
	}

	std::string hex(unicode_t* codepoints, size_t codepointsSize)
	{
		std::string converted;

		int32_t errors;
		size_t size_in_bytes = utf32toutf8(codepoints, codepointsSize, nullptr, 0, &errors);

		if (size_in_bytes == 0 ||
			errors != UTF8_ERR_NONE)
		{
			return converted;
		}

		converted.resize(size_in_bytes);
		utf32toutf8(codepoints, codepointsSize, &converted[0], size_in_bytes, nullptr);

		return hex(converted);
	}

	std::string hex(const std::string& text)
	{
		std::stringstream ss;

		for (std::string::const_iterator it = text.begin(); it != text.end(); ++it)
		{
			ss << "\\x" << std::uppercase << std::setfill('0') << std::hex << std::setw(2) << ((unicode_t)*it & 0x000000FF);
		}

		return ss.str();
	}

	std::string printable(unicode_t codepoint)
	{
		return printable(utf8(codepoint));
	}

	std::string printable(unicode_t* codepoints, size_t codepointsSize)
	{
		std::string converted;

		int32_t errors;
		size_t size_in_bytes = utf32toutf8(codepoints, codepointsSize, nullptr, 0, &errors);

		if (size_in_bytes == 0 ||
			errors != UTF8_ERR_NONE)
		{
			return converted;
		}

		converted.resize(size_in_bytes);
		utf32toutf8(codepoints, codepointsSize, &converted[0], size_in_bytes, nullptr);

		return printable(converted);
	}

	std::string printable(const std::string& text)
	{
		std::stringstream ss;

		bool wrote_hex = false;

		for (std::string::const_iterator it = text.begin(); it != text.end(); ++it)
		{
			printable(ss, wrote_hex, (uint8_t)*it);
		}

		return ss.str();
	}

	void canonicalCombiningClass(std::stringstream& target, unicode_t codepoint)
	{
	#if UTF8_VERSION_GUARD(1, 3, 0)
		target << (int)PROPERTY_GET_CCC(codepoint);
	#else
		target << database_queryproperty(codepoint, UnicodeProperty_CanonicalCombiningClass);
	#endif
	}

	std::string canonicalCombiningClass(unicode_t codepoint)
	{
		std::stringstream ss;
		canonicalCombiningClass(ss, codepoint);
		return ss.str();
	}

	std::string canonicalCombiningClass(unicode_t* codepoint, size_t codepointsSize)
	{
		std::stringstream ss;

		canonicalCombiningClass(ss, codepoint[0]);

		for (size_t i = 1; i < codepointsSize / sizeof(unicode_t); ++i)
		{
			ss << " ";
			canonicalCombiningClass(ss, codepoint[i]);
		}

		return ss.str();
	}

	std::string canonicalCombiningClass(const std::string& text)
	{
		std::vector<unicode_t> converted = utf32(text);
		if (converted.size() == 0)
		{
			return "";
		}

		std::stringstream ss;

		for (std::vector<unicode_t>::iterator it = converted.begin(); it != converted.end(); ++it)
		{
			if (it != converted.begin())
			{
				ss << " ";
			}
			canonicalCombiningClass(ss, *it);
		}

		return ss.str();
	}

	void quickCheck(std::stringstream& target, unicode_t codepoint, QuickCheck type)
	{
		uint8_t qc;

	#if UTF8_VERSION_GUARD(1, 3, 0)
		switch (type)
		{

		case QuickCheck::NFC:
			qc = PROPERTY_GET_NFC(codepoint);
			break;

		case QuickCheck::NFD:
			qc = PROPERTY_GET_NFD(codepoint);
			break;

		case QuickCheck::NFKC:
			qc = PROPERTY_GET_NFKC(codepoint);
			break;

		case QuickCheck::NFKD:
			qc = PROPERTY_GET_NFKD(codepoint);
			break;

		case QuickCheck::Any:
			qc = QuickCheckResult_Yes;
			break;

		default:
			break;

		}
	#else
		switch (type)
		{

		case QuickCheck::NFC:
			qc = database_queryproperty(codepoint, UnicodeProperty_Normalization_Compose);
			break;

		case QuickCheck::NFD:
			qc = database_queryproperty(codepoint, UnicodeProperty_Normalization_Decompose);
			break;

		case QuickCheck::NFKC:
			qc = database_queryproperty(codepoint, UnicodeProperty_Normalization_Compatibility_Compose);
			break;

		case QuickCheck::NFKD:
			qc = database_queryproperty(codepoint, UnicodeProperty_Normalization_Compatibility_Decompose);
			break;

		case QuickCheck::Any:
			qc = QuickCheckResult_Yes;
			break;

		default:
			break;

		}
	#endif

		switch (qc)
		{

		case QuickCheckResult_Yes:
			target << "Y";
			break;

		case QuickCheckResult_Maybe:
			target << "M";
			break;

		case QuickCheckResult_No:
			target << "N";
			break;

		default:
			target << "<invalid> (0x";
			target << std::hex << std::setfill('0') << std::setw(2) << std::uppercase << (int)qc;
			target << ")";
			break;

		}
	}

	std::string quickCheck(unicode_t codepoint, QuickCheck type)
	{
		std::stringstream ss;
		quickCheck(ss, codepoint, type);
		return ss.str();
	}

	std::string quickCheck(unicode_t* codepoint, size_t codepointsSize, QuickCheck type)
	{
		std::stringstream ss;

		quickCheck(ss, codepoint[0], type);

		for (size_t i = 1; i < codepointsSize / sizeof(unicode_t); ++i)
		{
			ss << " ";
			quickCheck(ss, codepoint[i], type);
		}

		return ss.str();
	}

	std::string quickCheck(const std::string& text, QuickCheck type)
	{
		std::vector<unicode_t> converted = utf32(text);
		if (converted.size() == 0)
		{
			return "";
		}

		std::stringstream ss;

		for (std::vector<unicode_t>::iterator it = converted.begin(); it != converted.end(); ++it)
		{
			if (it != converted.begin())
			{
				ss << " ";
			}
			quickCheck(ss, *it, type);
		}

		return ss.str();
	}

#if UTF8_VERSION_GUARD(1, 4, 0)
	std::string generalCategory(size_t flags)
	{
		struct Entry
		{
			size_t flag;
			const char* description;
		};

	#define MAKE_ENTRY(_name) { UTF8_CATEGORY_ ## _name, # _name }
		static const Entry EntriesMap[] = {
			MAKE_ENTRY(LETTER_UPPERCASE),
			MAKE_ENTRY(LETTER_LOWERCASE),
			MAKE_ENTRY(LETTER_TITLECASE),
			MAKE_ENTRY(LETTER_MODIFIER),
			MAKE_ENTRY(LETTER_OTHER),
			MAKE_ENTRY(MARK_NON_SPACING),
			MAKE_ENTRY(MARK_SPACING),
			MAKE_ENTRY(MARK_ENCLOSING),
			MAKE_ENTRY(NUMBER_DECIMAL),
			MAKE_ENTRY(NUMBER_LETTER),
			MAKE_ENTRY(NUMBER_OTHER),
			MAKE_ENTRY(PUNCTUATION_CONNECTOR),
			MAKE_ENTRY(PUNCTUATION_DASH),
			MAKE_ENTRY(PUNCTUATION_OPEN),
			MAKE_ENTRY(PUNCTUATION_CLOSE),
			MAKE_ENTRY(PUNCTUATION_INITIAL),
			MAKE_ENTRY(PUNCTUATION_FINAL),
			MAKE_ENTRY(PUNCTUATION_OTHER),
			MAKE_ENTRY(SYMBOL_MATH),
			MAKE_ENTRY(SYMBOL_CURRENCY),
			MAKE_ENTRY(SYMBOL_MODIFIER),
			MAKE_ENTRY(SYMBOL_OTHER),
			MAKE_ENTRY(SEPARATOR_SPACE),
			MAKE_ENTRY(SEPARATOR_LINE),
			MAKE_ENTRY(SEPARATOR_PARAGRAPH),
			MAKE_ENTRY(CONTROL),
			MAKE_ENTRY(FORMAT),
			MAKE_ENTRY(SURROGATE),
			MAKE_ENTRY(PRIVATE_USE),
			MAKE_ENTRY(UNASSIGNED),
		};
	#undef MAKE_ENTRY
		static const size_t EntriesMapSize = sizeof(EntriesMap) / sizeof(Entry);

		std::string output;
		size_t hit = 0;

		for (size_t i = 0; i < EntriesMapSize; ++i)
		{
			if ((EntriesMap[i].flag & flags) != 0)
			{
				if (hit++ != 0)
				{
					output += " | ";
				}

				output += EntriesMap[i].description;
			}
		}

		return output;
	}
#endif

	::testing::AssertionResult CompareUtf8Strings(
		const char* expressionExpected GTEST_ATTRIBUTE_UNUSED_, const char* expressionActual GTEST_ATTRIBUTE_UNUSED_,
		const char* textExpected, const char* textActual)
	{
		if (!strcmp(textActual, textExpected))
		{
			return ::testing::AssertionSuccess();
		}
		else
		{
			::testing::AssertionResult result = ::testing::AssertionFailure();

			result << "String mismatch" << std::endl;

			result << std::endl;

			result << "[UTF-8]" << std::endl;
			result << "    Actual: " << "\"" << printable(textActual) << "\"" << std::endl;
			result << "  Expected: " << "\"" << printable(textExpected) << "\"" << std::endl;

			result << std::endl;

			result << "[Codepoints]" << std::endl;
			result << "    Actual: " << "\"" << identifiable(textActual) << "\"" << std::endl;
			result << "  Expected: " << "\"" << identifiable(textExpected) << "\"" << std::endl;

			return result;
		}
	}

	::testing::AssertionResult CompareUtf8LengthStrings(
		const char* expressionExpected GTEST_ATTRIBUTE_UNUSED_, const char* expressionActual GTEST_ATTRIBUTE_UNUSED_, const char* expressionLength GTEST_ATTRIBUTE_UNUSED_,
		const char* textExpected, const char* textActual, size_t length)
	{
		if (!strncmp(textActual, textExpected, length))
		{
			return ::testing::AssertionSuccess();
		}
		else
		{
			::testing::AssertionResult result = ::testing::AssertionFailure();

			result << "String mismatch" << std::endl;

			result << std::endl;

			result << "[UTF-8]" << std::endl;
			result << "    Actual: " << "\"" << printable(std::string(textActual, length)) << "\"" << std::endl;
			result << "  Expected: " << "\"" << printable(textExpected) << "\"" << std::endl;

			result << std::endl;

			result << "[Codepoints]" << std::endl;
			result << "    Actual: " << "\"" << identifiable(std::string(textActual, length)) << "\"" << std::endl;
			result << "  Expected: " << "\"" << identifiable(textExpected) << "\"" << std::endl;

			return result;
		}
	}

	::testing::AssertionResult CompareOffsets(
		const char* expressionExpected GTEST_ATTRIBUTE_UNUSED_, const char* expressionActual GTEST_ATTRIBUTE_UNUSED_, const char* expressionCount GTEST_ATTRIBUTE_UNUSED_,
		const char* offsetExpected, const char* offsetActual, const char* offsetStart)
	{
		if (offsetExpected == offsetActual)
		{
			return ::testing::AssertionSuccess();
		}
		else
		{
			::testing::AssertionResult result = ::testing::AssertionFailure();

			result << "Offset mismatch." << std::endl;

			result << std::endl;

			result
				<< "[Offset]" << std::endl
				<< "    Actual: " << (ptrdiff_t)(offsetActual - offsetStart) << std::endl
				<< "  Expected: " << (ptrdiff_t)(offsetExpected - offsetStart) << std::endl;

			result << std::endl;

			result
				<< "[Text]" << std::endl
				<< "    Actual: \"" << printable(offsetActual) << "\"" << std::endl
				<< "  Expected: \"" << printable(offsetExpected) << "\"" << std::endl;

			return result;
		}
	}

	::testing::AssertionResult CompareMemory(
		const char* expressionExpected GTEST_ATTRIBUTE_UNUSED_, const char* expressionActual GTEST_ATTRIBUTE_UNUSED_, const char* expressionCount GTEST_ATTRIBUTE_UNUSED_,
		const char* memoryExpected, const char* memoryActual, size_t memorySize)
	{
		if (!memcmp(memoryExpected, memoryActual, memorySize))
		{
			return ::testing::AssertionSuccess();
		}
		else
		{
			::testing::AssertionResult result = ::testing::AssertionFailure();

			result << "Memory mismatch" << std::endl;

			const char* src_a = memoryActual;
			const char* src_e = memoryExpected;

			for (size_t i = 0; i < memorySize; ++i)
			{
				if (*src_a != *src_e)
				{
					result << "[Index " << i << "]" << std::endl;

					std::stringstream ssa;
					ssa << "0x" << std::uppercase << std::setfill('0') << std::hex << std::setw(2) << ((uint32_t)*src_a & 0x000000FF);
					result << "    Actual: " << ssa.str() << std::endl;

					std::stringstream sse;
					sse << "0x" << std::uppercase << std::setfill('0') << std::hex << std::setw(2) << ((uint32_t)*src_e & 0x000000FF);
					result << "  Expected: " << sse.str() << std::endl;
				}

				++src_a;
				++src_e;
			}

			return result;
		}
	}

	::testing::AssertionResult CompareCodepoints(
		const char* expressionExpected GTEST_ATTRIBUTE_UNUSED_, const char* expressionActual GTEST_ATTRIBUTE_UNUSED_,
		unicode_t codepointExpected, unicode_t codepointActual)
	{
		if (codepointActual == codepointExpected)
		{
			return ::testing::AssertionSuccess();
		}
		else
		{
			::testing::AssertionResult result = ::testing::AssertionFailure();

			result << "Codepoint mismatch" << std::endl;

			result << "    Actual: " << identifiable(codepointActual) << " \"" << printable(codepointActual) << "\"" << std::endl;
			result << "  Expected: " << identifiable(codepointExpected) << " \"" << printable(codepointExpected) << "\"" << std::endl;

			return result;
		}
	}

#if UTF8_VERSION_GUARD(1, 4, 0)
	::testing::AssertionResult CompareGeneralCategory(
		const char* expressionExpected GTEST_ATTRIBUTE_UNUSED_, const char* expressionActual GTEST_ATTRIBUTE_UNUSED_,
		const GeneralCategoryEntry& entryExpected, const GeneralCategoryEntry& entryActual)
	{
		if ((entryExpected.standard != -1 && (entryExpected.standard > 0) == (entryActual.offset > 0)) ||
			entryExpected.offset == entryActual.offset)
		{
			return ::testing::AssertionSuccess();
		}
		else
		{
			::testing::AssertionResult result = ::testing::AssertionFailure();

			result << "Category mismatch " << identifiable(entryActual.input) << " \"" << printable(entryActual.input) << "\"" << std::endl;

			result << std::endl;

			if (entryExpected.standard != -1)
			{
				if ((entryExpected.standard > 0) != (entryActual.offset > 0))
				{
					result << "[Standard]" << std::endl;
					result << "    Actual:  " << entryActual.offset << std::endl;
					result << "  Expected:  " << entryExpected.offset << " (" << entryExpected.standardName << ")" << std::endl;
				}
				else
				{
					result << "[Standard]   " << entryExpected.offset << " (" << entryExpected.standardName << ")" << std::endl;
				}

				result << std::endl;
			}

			if (entryExpected.offset != entryActual.offset)
			{
				result << "[Offset]" << std::endl;
				result << "    Actual:  " << entryActual.offset << std::endl;
				result << "  Expected:  " << entryExpected.offset << std::endl;
			}
			else
			{
				result << "[Offset]     " << entryActual.offset << std::endl;
			}

			result << std::endl;

			size_t general_category = 0;

			unicode_t code_point;
			if (codepoint_read(entryActual.input, entryActual.inputSize, &code_point) != 0)
			{
				general_category = PROPERTY_GET_GC(code_point);
			}

			if ((general_category & entryExpected.flags) == 0)
			{
				result << "[Flags]" << std::endl;
				result << "    Actual:  " << generalCategory(general_category) << std::endl;
				result << "  Expected:  " << generalCategory(entryExpected.flags) << std::endl;
			}
			else
			{
				result << "[Flags]      " << generalCategory(general_category) << std::endl;
			}
			

			return result;
		}
	}
#endif

};