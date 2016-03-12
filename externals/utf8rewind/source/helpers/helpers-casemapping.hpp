#pragma once

/*!
	\file
	\brief Case mapping helper functions.

	\cond INTERNAL
*/

#include "helpers-base.hpp"

#define EXPECT_CASEMAPPING_CODEPOINT_NUL_EQ(_codepoint, _name) { \
	::helpers::CaseMappingEntry e; \
	e.codepoint = _codepoint; \
	e.lowercase = std::string(1, '\0'); \
	e.uppercase = std::string(1, '\0'); \
	e.titlecase = std::string(1, '\0'); \
	e.name = _name; \
	::helpers::CaseMappingEntry a; \
	a.lowercase = ::helpers::lowercase(_codepoint); \
	a.uppercase = ::helpers::uppercase(_codepoint); \
	a.titlecase = ::helpers::titlecase(_codepoint); \
	EXPECT_PRED_FORMAT2(::helpers::CompareCodepoint, e, a); \
}

#define EXPECT_CASEMAPPING_CODEPOINT_EQ(_codepoint, _lowercase, _uppercase, _titlecase, _name) { \
	::helpers::CaseMappingEntry e; \
	e.codepoint = _codepoint; \
	e.lowercase = _lowercase; \
	e.uppercase = _uppercase; \
	e.titlecase = _titlecase; \
	e.name = _name; \
	::helpers::CaseMappingEntry a; \
	a.lowercase = ::helpers::lowercase(_codepoint); \
	a.uppercase = ::helpers::uppercase(_codepoint); \
	a.titlecase = ::helpers::titlecase(_codepoint); \
	EXPECT_PRED_FORMAT2(::helpers::CompareCodepoint, e, a); \
}

#define EXPECT_CASEMAPPING_EQ(_input, _lowercase, _uppercase, _titlecase) { \
	::helpers::CaseMappingEntry e; \
	e.lowercase = _lowercase; \
	e.uppercase = _uppercase; \
	e.titlecase = _titlecase; \
	::helpers::CaseMappingEntry a; \
	a.input = _input; \
	a.lowercase = ::helpers::lowercase(_input); \
	a.uppercase = ::helpers::uppercase(_input); \
	a.titlecase = ::helpers::titlecase(_input); \
	EXPECT_PRED_FORMAT2(::helpers::CompareCaseMapping, e, a); \
}

#if UTF8_VERSION_GUARD(1, 4, 0)
	#define EXPECT_CASEFOLDING_EQ(_codepoint, _folded, _name) { \
		::helpers::CaseFoldingEntry e; \
		e.codePoint = _codepoint; \
		e.folded = _folded; \
		e.name = _name; \
		::helpers::CaseFoldingEntry a; \
		a.folded = ::helpers::casefold(_codepoint); \
		EXPECT_PRED_FORMAT2(::helpers::CompareCaseFolding, e, a); \
	}
#endif

namespace helpers {

	std::string uppercase(unicode_t codepoint);
	std::string uppercase(const std::string& text);

	std::string lowercase(unicode_t codepoint);
	std::string lowercase(const std::string& text);

	std::string titlecase(unicode_t codepoint);
	std::string titlecase(const std::string& text);

#if UTF8_VERSION_GUARD(1, 4, 0)
	std::string casefold(unicode_t codepoint);
	std::string casefold(const std::string& text);
#endif

	struct CaseMappingEntry
	{
		CaseMappingEntry()
			: codepoint(0)
		{
		}

		unicode_t codepoint;
		std::string name;
		std::string input;
		std::string lowercase;
		std::string uppercase;
		std::string titlecase;
	};

	::testing::AssertionResult CompareCodepoint(
		const char* expressionExpected, const char* expressionActual,
		const CaseMappingEntry& entryExpected, const CaseMappingEntry& entryActual);

	::testing::AssertionResult CompareCaseMapping(
		const char* expressionExpected, const char* expressionActual,
		const CaseMappingEntry& entryExpected, const CaseMappingEntry& entryActual);

#if UTF8_VERSION_GUARD(1, 4, 0)
	struct CaseFoldingEntry
	{
		unicode_t codePoint;
		std::string folded;
		std::string name;
	};

	::testing::AssertionResult CompareCaseFolding(
		const char* expressionExpected, const char* expressionActual,
		const CaseFoldingEntry& entryExpected, const CaseFoldingEntry& entryActual);
#endif

};

/*! \endcond */