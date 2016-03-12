#include "tests-base.hpp"

#include "../helpers/helpers-locale.hpp"
#include "../helpers/helpers-strings.hpp"

class Utf8ToUpperTurkish
	: public ::testing::Test
{

protected:

	void SetUp()
	{
		SET_LOCALE_TURKISH();
	}

	void TearDown()
	{
		RESET_LOCALE();
	}

};

TEST_F(Utf8ToUpperTurkish, SingleCapitalLetterI)
{
	// 0049
	// 0049

	const char* c = "I";
	const size_t s = 255;
	char b[256] = { 0 };
	int32_t errors = UTF8_ERR_NONE;

	EXPECT_EQ(1, utf8toupper(c, strlen(c), b, s, &errors));
	EXPECT_UTF8EQ("I", b);
	EXPECT_ERROREQ(UTF8_ERR_NONE, errors);
}

TEST_F(Utf8ToUpperTurkish, SingleCapitalLetterINotEnoughSpace)
{
	const char* c = "I";
	const size_t s = 0;
	char b[256] = { 0 };
	int32_t errors = UTF8_ERR_NONE;

	EXPECT_EQ(0, utf8toupper(c, strlen(c), b, s, &errors));
	EXPECT_UTF8EQ("", b);
	EXPECT_ERROREQ(UTF8_ERR_NOT_ENOUGH_SPACE, errors);
}

TEST_F(Utf8ToUpperTurkish, SingleCapitalLetterIAmountOfBytes)
{
	const char* c = "I";
	int32_t errors = UTF8_ERR_NONE;

	EXPECT_EQ(1, utf8toupper(c, strlen(c), nullptr, 0, &errors));
	EXPECT_ERROREQ(UTF8_ERR_NONE, errors);
}

TEST_F(Utf8ToUpperTurkish, SingleCapitalLetterIAndDotAbove)
{
	// 0049 0307
	// 0049 0307

	const char* c = "I\xCC\x87";
	const size_t s = 255;
	char b[256] = { 0 };
	int32_t errors = UTF8_ERR_NONE;

	EXPECT_EQ(3, utf8toupper(c, strlen(c), b, s, &errors));
	EXPECT_UTF8EQ("I\xCC\x87", b);
	EXPECT_ERROREQ(UTF8_ERR_NONE, errors);
}

TEST_F(Utf8ToUpperTurkish, SingleCapitalLetterIAndDotAboveNotEnoughSpace)
{
	const char* c = "I\xCC\x87";
	const size_t s = 2;
	char b[256] = { 0 };
	int32_t errors = UTF8_ERR_NONE;

	EXPECT_EQ(1, utf8toupper(c, strlen(c), b, s, &errors));
	EXPECT_UTF8EQ("I", b);
	EXPECT_ERROREQ(UTF8_ERR_NOT_ENOUGH_SPACE, errors);
}

TEST_F(Utf8ToUpperTurkish, SingleCapitalLetterIAndDotAboveAmountOfBytes)
{
	const char* c = "I\xCC\x87";
	int32_t errors = UTF8_ERR_NONE;

	EXPECT_EQ(3, utf8toupper(c, strlen(c), nullptr, 0, &errors));
	EXPECT_ERROREQ(UTF8_ERR_NONE, errors);
}

TEST_F(Utf8ToUpperTurkish, SingleCapitalLetterIWithDotAbove)
{
	// 0130
	// 0130

	const char* c = "\xC4\xB0";
	const size_t s = 255;
	char b[256] = { 0 };
	int32_t errors = UTF8_ERR_NONE;

	EXPECT_EQ(2, utf8toupper(c, strlen(c), b, s, &errors));
	EXPECT_UTF8EQ("\xC4\xB0", b);
	EXPECT_ERROREQ(UTF8_ERR_NONE, errors);
}

TEST_F(Utf8ToUpperTurkish, SingleCapitalLetterIWithDotAboveNotEnoughSpace)
{
	const char* c = "\xC4\xB0";
	const size_t s = 1;
	char b[256] = { 0 };
	int32_t errors = UTF8_ERR_NONE;

	EXPECT_EQ(0, utf8toupper(c, strlen(c), b, s, &errors));
	EXPECT_UTF8EQ("", b);
	EXPECT_ERROREQ(UTF8_ERR_NOT_ENOUGH_SPACE, errors);
}

TEST_F(Utf8ToUpperTurkish, SingleCapitalLetterIWithDotAboveAmountOfBytes)
{
	const char* c = "\xC4\xB0";
	int32_t errors = UTF8_ERR_NONE;

	EXPECT_EQ(2, utf8toupper(c, strlen(c), nullptr, 0, &errors));
	EXPECT_ERROREQ(UTF8_ERR_NONE, errors);
}

TEST_F(Utf8ToUpperTurkish, SingleCapitalLetterIWithDotAboveAndDotAbove)
{
	// 0130 0307
	// 0130 0307

	const char* c = "\xC4\xB0\xCC\x87";
	const size_t s = 255;
	char b[256] = { 0 };
	int32_t errors = UTF8_ERR_NONE;

	EXPECT_EQ(4, utf8toupper(c, strlen(c), b, s, &errors));
	EXPECT_UTF8EQ("\xC4\xB0\xCC\x87", b);
	EXPECT_ERROREQ(UTF8_ERR_NONE, errors);
}

TEST_F(Utf8ToUpperTurkish, SingleCapitalLetterIWithDotAboveAndDotAboveNotEnoughSpace)
{
	const char* c = "\xC4\xB0\xCC\x87";
	const size_t s = 3;
	char b[256] = { 0 };
	int32_t errors = UTF8_ERR_NONE;

	EXPECT_EQ(2, utf8toupper(c, strlen(c), b, s, &errors));
	EXPECT_UTF8EQ("\xC4\xB0", b);
	EXPECT_ERROREQ(UTF8_ERR_NOT_ENOUGH_SPACE, errors);
}

TEST_F(Utf8ToUpperTurkish, SingleCapitalLetterIWithDotAboveAndDotAboveAmountOfBytes)
{
	const char* c = "\xC4\xB0\xCC\x87";
	int32_t errors = UTF8_ERR_NONE;

	EXPECT_EQ(4, utf8toupper(c, strlen(c), nullptr, 0, &errors));
	EXPECT_ERROREQ(UTF8_ERR_NONE, errors);
}

TEST_F(Utf8ToUpperTurkish, SingleSmallLetterI)
{
	// 0069
	// 0130

	const char* c = "i";
	const size_t s = 255;
	char b[256] = { 0 };
	int32_t errors = UTF8_ERR_NONE;

	EXPECT_EQ(2, utf8toupper(c, strlen(c), b, s, &errors));
	EXPECT_UTF8EQ("\xC4\xB0", b);
	EXPECT_ERROREQ(UTF8_ERR_NONE, errors);
}

TEST_F(Utf8ToUpperTurkish, SingleSmallLetterINotEnoughSpace)
{
	const char* c = "i";
	const size_t s = 0;
	char b[256] = { 0 };
	int32_t errors = UTF8_ERR_NONE;

	EXPECT_EQ(0, utf8toupper(c, strlen(c), b, s, &errors));
	EXPECT_UTF8EQ("", b);
	EXPECT_ERROREQ(UTF8_ERR_NOT_ENOUGH_SPACE, errors);
}

TEST_F(Utf8ToUpperTurkish, SingleSmallLetterIAmountOfBytes)
{
	const char* c = "i";
	int32_t errors = UTF8_ERR_NONE;

	EXPECT_EQ(2, utf8toupper(c, strlen(c), nullptr, 0, &errors));
	EXPECT_ERROREQ(UTF8_ERR_NONE, errors);
}

TEST_F(Utf8ToUpperTurkish, SingleSmallLetterIWithDotAbove)
{
	// 0069 0307
	// 0130 0307

	const char* c = "i\xCC\x87";
	const size_t s = 255;
	char b[256] = { 0 };
	int32_t errors = UTF8_ERR_NONE;

	EXPECT_EQ(4, utf8toupper(c, strlen(c), b, s, &errors));
	EXPECT_UTF8EQ("\xC4\xB0\xCC\x87", b);
	EXPECT_ERROREQ(UTF8_ERR_NONE, errors);
}

TEST_F(Utf8ToUpperTurkish, SingleSmallLetterIWithDotAboveNotEnoughSpace)
{
	const char* c = "i\xCC\x87";
	const size_t s = 3;
	char b[256] = { 0 };
	int32_t errors = UTF8_ERR_NONE;

	EXPECT_EQ(2, utf8toupper(c, strlen(c), b, s, &errors));
	EXPECT_UTF8EQ("\xC4\xB0", b);
	EXPECT_ERROREQ(UTF8_ERR_NOT_ENOUGH_SPACE, errors);
}

TEST_F(Utf8ToUpperTurkish, SingleSmallLetterIWithDotAboveAmountOfBytes)
{
	const char* c = "i\xCC\x87";
	int32_t errors = UTF8_ERR_NONE;

	EXPECT_EQ(4, utf8toupper(c, strlen(c), nullptr, 0, &errors));
	EXPECT_ERROREQ(UTF8_ERR_NONE, errors);
}

TEST_F(Utf8ToUpperTurkish, SingleSmallLetterDotlessI)
{
	// 0131
	// 0049

	const char* c = "\xC4\xB1";
	const size_t s = 255;
	char b[256] = { 0 };
	int32_t errors = UTF8_ERR_NONE;

	EXPECT_EQ(1, utf8toupper(c, strlen(c), b, s, &errors));
	EXPECT_UTF8EQ("I", b);
	EXPECT_ERROREQ(UTF8_ERR_NONE, errors);
}

TEST_F(Utf8ToUpperTurkish, SingleSmallLetterDotlessINotEnoughSpace)
{
	const char* c = "\xC4\xB1";
	const size_t s = 0;
	char b[256] = { 0 };
	int32_t errors = UTF8_ERR_NONE;

	EXPECT_EQ(0, utf8toupper(c, strlen(c), b, s, &errors));
	EXPECT_UTF8EQ("", b);
	EXPECT_ERROREQ(UTF8_ERR_NOT_ENOUGH_SPACE, errors);
}

TEST_F(Utf8ToUpperTurkish, SingleSmallLetterDotlessIAmountOfBytes)
{
	const char* c = "\xC4\xB1";
	int32_t errors = UTF8_ERR_NONE;

	EXPECT_EQ(1, utf8toupper(c, strlen(c), nullptr, 0, &errors));
	EXPECT_ERROREQ(UTF8_ERR_NONE, errors);
}

TEST_F(Utf8ToUpperTurkish, SingleSmallLetterDotlessIWithDotAbove)
{
	// 0131 0307
	// 0049 0307

	const char* c = "\xC4\xB1\xCC\x87";
	const size_t s = 255;
	char b[256] = { 0 };
	int32_t errors = UTF8_ERR_NONE;

	EXPECT_EQ(3, utf8toupper(c, strlen(c), b, s, &errors));
	EXPECT_UTF8EQ("I\xCC\x87", b);
	EXPECT_ERROREQ(UTF8_ERR_NONE, errors);
}

TEST_F(Utf8ToUpperTurkish, SingleSmallLetterDotlessIWithDotAboveNotEnoughSpace)
{
	const char* c = "\xC4\xB1\xCC\x87";
	const size_t s = 2;
	char b[256] = { 0 };
	int32_t errors = UTF8_ERR_NONE;

	EXPECT_EQ(1, utf8toupper(c, strlen(c), b, s, &errors));
	EXPECT_UTF8EQ("I", b);
	EXPECT_ERROREQ(UTF8_ERR_NOT_ENOUGH_SPACE, errors);
}

TEST_F(Utf8ToUpperTurkish, SingleSmallLetterDotlessIWithDotAboveAmountOfBytes)
{
	const char* c = "\xC4\xB1\xCC\x87";
	int32_t errors = UTF8_ERR_NONE;

	EXPECT_EQ(3, utf8toupper(c, strlen(c), nullptr, 0, &errors));
	EXPECT_ERROREQ(UTF8_ERR_NONE, errors);
}