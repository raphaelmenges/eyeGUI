#pragma once

/*!
	\file
	\brief Normalization helper functions.

	\cond INTERNAL
*/

#include "helpers-base.hpp"

#define CHECK_IS_NORMALIZED(_codepoint, _nfd, _nfc, _nfkd, _nfkc, _name) { \
	::helpers::CheckEntry e; \
	e.codepoint = _codepoint; \
	e.name = _name; \
	e.nfd = UTF8_NORMALIZATION_RESULT_ ## _nfd; \
	e.nfc = UTF8_NORMALIZATION_RESULT_ ## _nfc; \
	e.nfkd = UTF8_NORMALIZATION_RESULT_ ## _nfkd; \
	e.nfkc = UTF8_NORMALIZATION_RESULT_ ## _nfkc; \
	::helpers::CheckEntry a; \
	a.nfd = ::helpers::isNfd(_codepoint); \
	a.nfc = ::helpers::isNfc(_codepoint); \
	a.nfkd = ::helpers::isNfkd(_codepoint); \
	a.nfkc = ::helpers::isNfkc(_codepoint); \
	EXPECT_PRED_FORMAT2(::helpers::CompareNormalizationCheck, e, a); \
}

#define CHECK_NORMALIZE_CODEPOINT(_codepoint, _decomposed, _composed, _decomposedCompatibility, _composedCompatibility, _name) { \
	::helpers::NormalizationEntry e; \
	e.codepoint = _codepoint; \
	e.name = _name; \
	e.decomposed = _decomposed; \
	e.composed = _composed; \
	e.decomposedCompatibility = _decomposedCompatibility; \
	e.composedCompatibility = _composedCompatibility; \
	::helpers::NormalizationEntry a; \
	a.decomposed = helpers::nfd(_codepoint); \
	a.composed = helpers::nfc(_codepoint); \
	a.decomposedCompatibility = helpers::nfkd(_codepoint); \
	a.composedCompatibility = helpers::nfkc(_codepoint); \
	EXPECT_PRED_FORMAT2(::helpers::CompareNormalizationCodepoint, e, a); \
}

#define CHECK_NORMALIZE_SEQUENCE(_sequence, _decomposed, _composed, _decomposedCompatibility, _composedCompatibility) { \
	::helpers::NormalizationEntry e; \
	e.sequence = _sequence; \
	e.decomposed = _decomposed; \
	e.composed = _composed; \
	e.decomposedCompatibility = _decomposedCompatibility; \
	e.composedCompatibility = _composedCompatibility; \
	::helpers::NormalizationEntry a; \
	a.decomposed = helpers::nfd(_sequence); \
	a.composed = helpers::nfc(_sequence); \
	a.decomposedCompatibility = helpers::nfkd(_sequence); \
	a.composedCompatibility = helpers::nfkc(_sequence); \
	EXPECT_PRED_FORMAT2(::helpers::CompareNormalizationSequence, e, a); \
}

namespace helpers {

	std::string normalizationResult(uint8_t result);

	uint8_t isNfc(unicode_t codepoint);
	uint8_t isNfc(const std::string& text);

	std::string nfc(unicode_t codepoint);
	std::string nfc(const std::string& text);

	uint8_t isNfd(unicode_t codepoint);
	uint8_t isNfd(const std::string& text);

	std::string nfd(unicode_t codepoint);
	std::string nfd(const std::string& text);

	uint8_t isNfkc(unicode_t codepoint);
	uint8_t isNfkc(const std::string& text);

	std::string nfkc(unicode_t codepoint);
	std::string nfkc(const std::string& text);

	uint8_t isNfkd(unicode_t codepoint);
	uint8_t isNfkd(const std::string& text);

	std::string nfkd(unicode_t codepoint);
	std::string nfkd(const std::string& text);

	struct CheckEntry
	{
		unicode_t codepoint;
		std::string name;
		uint8_t nfd;
		uint8_t nfc;
		uint8_t nfkd;
		uint8_t nfkc;
	};

	::testing::AssertionResult CompareNormalizationCheck(
		const char* expressionExpected, const char* expressionActual,
		const CheckEntry& entryExpected, const CheckEntry& entryActual);

	struct NormalizationEntry
	{
		NormalizationEntry()
			: codepoint(0)
		{
		}

		unicode_t codepoint;
		std::string sequence;
		std::string name;
		std::string decomposed;
		std::string composed;
		std::string decomposedCompatibility;
		std::string composedCompatibility;
	};

	::testing::AssertionResult CompareNormalizationCodepoint(
		const char* expressionExpected, const char* expressionActual,
		const NormalizationEntry& entryExpected, const NormalizationEntry& entryActual);

	::testing::AssertionResult CompareNormalizationSequence(
		const char* expressionExpected, const char* expressionActual,
		const NormalizationEntry& entryExpected, const NormalizationEntry& entryActual);

};

/*! \endcond */