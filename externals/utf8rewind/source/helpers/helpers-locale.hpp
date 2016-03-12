#pragma once

/*!
	\file
	\brief Locale helper functions.

	\cond INTERNAL
*/

#include "helpers-base.hpp"

#define EXPECT_LOCALE_EQ(_expected, _actual) \
	EXPECT_PRED_FORMAT2(::helpers::CompareLocale, _expected, _actual); \

#define RESET_LOCALE()               setlocale(LC_ALL, "C")

#if _WINDOWS
	#define SET_LOCALE_AZERI()       EXPECT_STREQ("Azeri_Azerbaijan.1254", setlocale(LC_ALL, "Azeri_Azerbaijan.1254"))
	#define SET_LOCALE_DANISH()      EXPECT_STREQ("Danish_Denmark.1252", setlocale(LC_ALL, "Danish_Denmark.1252"))
	#define SET_LOCALE_ENGLISH()     EXPECT_STREQ("en-US", setlocale(LC_ALL, "en-US"))
	#define SET_LOCALE_FRENCH()      EXPECT_STREQ("French_France.1252", setlocale(LC_ALL, "French_France.1252"))
	#define SET_LOCALE_GERMAN()      EXPECT_STREQ("German_Germany.1252", setlocale(LC_ALL, "German_Germany.1252"))
	#define SET_LOCALE_GREEK()       EXPECT_STREQ("Greek_Greece.1253", setlocale(LC_ALL, "Greek_Greece.1253"))
	#define SET_LOCALE_HEBREW()      EXPECT_STREQ("Hebrew_Israel.1255", setlocale(LC_ALL, "Hebrew_Israel.1255"))
	#define SET_LOCALE_HUNGARIAN()   EXPECT_STREQ("Hungarian_Hungary.1250", setlocale(LC_ALL, "Hungarian_Hungary.1250"))
	#define SET_LOCALE_ICELANDIC()   EXPECT_STREQ("Icelandic_Iceland.1252", setlocale(LC_ALL, "Icelandic_Iceland.1252"))
	#define SET_LOCALE_IRISH()       EXPECT_STREQ("Irish_Ireland.1252", setlocale(LC_ALL, "Irish_Ireland.1252"))
	#define SET_LOCALE_JAPANESE()    EXPECT_STREQ("Japanese_Japan.932", setlocale(LC_ALL, "Japanese_Japan.932"))
	#define SET_LOCALE_LITHUANIAN()  EXPECT_STREQ("Lithuanian_Lithuania.1257", setlocale(LC_ALL, "Lithuanian_Lithuania.1257"))
	#define SET_LOCALE_POLISH()      EXPECT_STREQ("Polish_Poland.1250", setlocale(LC_ALL, "Polish_Poland.1250"))
	#define SET_LOCALE_RUSSIAN()     EXPECT_STREQ("Russian_Russia.1251", setlocale(LC_ALL, "Russian_Russia.1251"))
	#define SET_LOCALE_SPANISH()     EXPECT_STREQ("Spanish_Spain.1252", setlocale(LC_ALL, "Spanish_Spain.1252"))
	#define SET_LOCALE_THAI()        EXPECT_STREQ("Thai_Thailand.874", setlocale(LC_ALL, "Thai_Thailand.874"))
	#define SET_LOCALE_TURKISH()     EXPECT_STREQ("Turkish_Turkey.1254", setlocale(LC_ALL, "Turkish_Turkey.1254"))
#else
	#define SET_LOCALE_AZERI()       EXPECT_STREQ("az_AZ.utf8", setlocale(LC_ALL, "az_AZ.utf8"))
	#define SET_LOCALE_DANISH()      EXPECT_STREQ("da_DK.utf8", setlocale(LC_ALL, "da_DK.utf8"))
	#define SET_LOCALE_ENGLISH()     EXPECT_STREQ("en_US.utf8", setlocale(LC_ALL, "en_US.utf8"))
	#define SET_LOCALE_FRENCH()      EXPECT_STREQ("fr_FR.utf8", setlocale(LC_ALL, "fr_FR.utf8"))
	#define SET_LOCALE_GERMAN()      EXPECT_STREQ("de_DE.utf8", setlocale(LC_ALL, "de_DE.utf8"))
	#define SET_LOCALE_GREEK()       EXPECT_STREQ("el_GR.utf8", setlocale(LC_ALL, "el_GR.utf8"))
	#define SET_LOCALE_HEBREW()      EXPECT_STREQ("he_IL.utf8", setlocale(LC_ALL, "he_IL.utf8"))
	#define SET_LOCALE_HUNGARIAN()   EXPECT_STREQ("hu_HU.utf8", setlocale(LC_ALL, "hu_HU.utf8"))
	#define SET_LOCALE_ICELANDIC()   EXPECT_STREQ("is_IS.utf8", setlocale(LC_ALL, "is_IS.utf8"))
	#define SET_LOCALE_IRISH()       EXPECT_STREQ("ga_IE.utf8", setlocale(LC_ALL, "ga_IE.utf8"))
	#define SET_LOCALE_JAPANESE()    EXPECT_STREQ("ja_JP.utf8", setlocale(LC_ALL, "ja_JP.utf8"))
	#define SET_LOCALE_LITHUANIAN()  EXPECT_STREQ("lt_LT.utf8", setlocale(LC_ALL, "lt_LT.utf8"))
	#define SET_LOCALE_POLISH()      EXPECT_STREQ("pl_PL.utf8", setlocale(LC_ALL, "pl_PL.utf8"))
	#define SET_LOCALE_RUSSIAN()     EXPECT_STREQ("ru_RU.utf8", setlocale(LC_ALL, "ru_RU.utf8"))
	#define SET_LOCALE_SPANISH()     EXPECT_STREQ("es_ES.utf8", setlocale(LC_ALL, "es_ES.utf8"))
	#define SET_LOCALE_THAI()        EXPECT_STREQ("th_TH.utf8", setlocale(LC_ALL, "th_TH.utf8"))
	#define SET_LOCALE_TURKISH()     EXPECT_STREQ("tr_TR.utf8", setlocale(LC_ALL, "tr_TR.utf8"))
#endif

namespace helpers {

	std::string locale(uint32_t value);

	::testing::AssertionResult CompareLocale(
		const char* expressionExpected, const char* expressionActual,
		uint32_t localeExpected, uint32_t localeActual);

};

/*! \endcond */