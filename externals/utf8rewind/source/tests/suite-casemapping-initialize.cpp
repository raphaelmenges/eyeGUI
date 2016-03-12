#include "tests-base.hpp"

extern "C" {
	#include "../internal/casemapping.h"
	#include "../internal/database.h"
}

#include "../helpers/helpers-casemapping.hpp"
#include "../helpers/helpers-locale.hpp"
#include "../helpers/helpers-strings.hpp"

TEST(CaseMappingInitialize, Initialize)
{
	CaseMappingState state;
	const char* i = "Greetings";
	size_t is = strlen(i);
	char o[256] = { 0 };
	size_t os = 255;

	EXPECT_TRUE(casemapping_initialize(&state, i, is, o, os, TitlecaseIndex1Ptr, TitlecaseIndex2Ptr, TitlecaseDataPtr));
	EXPECT_EQ(i, state.src);
	EXPECT_EQ(is, state.src_size);
	EXPECT_EQ(o, state.dst);
	EXPECT_EQ(os, state.dst_size);
	EXPECT_EQ(TitlecaseIndex1Ptr, state.property_index1);
	EXPECT_EQ(TitlecaseIndex2Ptr, state.property_index2);
	EXPECT_EQ(TitlecaseDataPtr, state.property_data);
}

TEST(CaseMappingInitialize, LocaleEnglishUS)
{
	CaseMappingState state;
	const char* i = "Pancakes";
	size_t is = strlen(i);
	char o[256] = { 0 };
	size_t os = 255;

	SET_LOCALE_ENGLISH();
	EXPECT_TRUE(casemapping_initialize(&state, i, is, o, os, TitlecaseIndex1Ptr, TitlecaseIndex2Ptr, TitlecaseDataPtr));
	EXPECT_LOCALE_EQ(CASEMAPPING_LOCALE_DEFAULT, state.locale);
	RESET_LOCALE();
}

TEST(CaseMappingInitialize, LocaleLithuanian)
{
	CaseMappingState state;
	const char* i = "Cuisine";
	size_t is = strlen(i);
	char o[256] = { 0 };
	size_t os = 255;

	SET_LOCALE_LITHUANIAN();
	EXPECT_TRUE(casemapping_initialize(&state, i, is, o, os, UppercaseIndex1Ptr, UppercaseIndex2Ptr, UppercaseDataPtr));
	EXPECT_LOCALE_EQ(CASEMAPPING_LOCALE_LITHUANIAN, state.locale);
	RESET_LOCALE();
}

TEST(CaseMappingInitialize, LocaleTurkish)
{
	CaseMappingState state;
	const char* i = "I welcome our Turkish friends.";
	size_t is = strlen(i);
	char o[256] = { 0 };
	size_t os = 255;

	SET_LOCALE_TURKISH();
	EXPECT_TRUE(casemapping_initialize(&state, i, is, o, os, UppercaseIndex1Ptr, UppercaseIndex2Ptr, UppercaseDataPtr));
	EXPECT_LOCALE_EQ(CASEMAPPING_LOCALE_TURKISH_OR_AZERI_LATIN, state.locale);
	RESET_LOCALE();
}

TEST(CaseMappingInitialize, LocaleAzeri)
{
	CaseMappingState state;
	const char* i = "Cuisine.";
	size_t is = strlen(i);
	char o[256] = { 0 };
	size_t os = 255;

	SET_LOCALE_AZERI();
	EXPECT_TRUE(casemapping_initialize(&state, i, is, o, os, UppercaseIndex1Ptr, UppercaseIndex2Ptr, UppercaseDataPtr));
	EXPECT_LOCALE_EQ(CASEMAPPING_LOCALE_TURKISH_OR_AZERI_LATIN, state.locale);
	RESET_LOCALE();
}