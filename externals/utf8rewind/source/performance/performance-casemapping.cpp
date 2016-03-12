#include "performance-base.hpp"

#include "../helpers/helpers-strings.hpp"

extern "C" {
	#include "../internal/codepoint.h"
}

class CaseMappingBasicLatin
	: public performance::Suite
{

public:

	virtual void setup() override
	{
		std::vector<unicode_t> codepoints;

		for (unicode_t i = 0; i <= MAX_BASIC_LATIN; ++i)
		{
			codepoints.push_back(i);
		}

		m_input = helpers::utf8(codepoints);
	}

	std::string m_input;

};

PERF_TEST_F(CaseMappingBasicLatin, LowercaseStatic)
{
	char o[1024] = { 0 };
	size_t ol = 1023;
	int32_t e;

	size_t l = utf8tolower(m_input.c_str(), m_input.length(), o, ol, &e);

	PERF_ASSERT(l > 0);
	PERF_ASSERT(e == UTF8_ERR_NONE);
}

PERF_TEST_F(CaseMappingBasicLatin, LowercaseDynamic)
{
	int32_t e;

	size_t ol = utf8tolower(m_input.c_str(), m_input.length(), nullptr, 0, &e);

	PERF_ASSERT(ol > 0);
	PERF_ASSERT(e == UTF8_ERR_NONE);

	if (ol > 0 &&
		e == UTF8_ERR_NONE)
	{
		char* o = new char[ol + 1];
		memset(o, 0, ol + 1);

		utf8tolower(m_input.c_str(), m_input.length(), o, ol, nullptr);

		delete [] o;
	}
}

PERF_TEST_F(CaseMappingBasicLatin, UppercaseStatic)
{
	char o[1024] = { 0 };
	size_t ol = 1023;
	int32_t e;

	size_t l = utf8toupper(m_input.c_str(), m_input.length(), o, ol, &e);

	PERF_ASSERT(l > 0);
	PERF_ASSERT(e == UTF8_ERR_NONE);
}

PERF_TEST_F(CaseMappingBasicLatin, UppercaseDynamic)
{
	int32_t e;

	size_t ol = utf8toupper(m_input.c_str(), m_input.length(), nullptr, 0, &e);

	PERF_ASSERT(ol > 0);
	PERF_ASSERT(e == UTF8_ERR_NONE);

	if (ol > 0 &&
		e == UTF8_ERR_NONE)
	{
		char* o = new char[ol + 1];
		memset(o, 0, ol + 1);

		utf8toupper(m_input.c_str(), m_input.length(), o, ol, nullptr);

		delete [] o;
	}
}

PERF_TEST_F(CaseMappingBasicLatin, TitlecaseStatic)
{
	char o[1024] = { 0 };
	size_t ol = 1023;
	int32_t e;

	size_t l = utf8totitle(m_input.c_str(), m_input.length(), o, ol, &e);

	PERF_ASSERT(l > 0);
	PERF_ASSERT(e == UTF8_ERR_NONE);
}

PERF_TEST_F(CaseMappingBasicLatin, TitlecaseDynamic)
{
	int32_t e;

	size_t ol = utf8totitle(m_input.c_str(), m_input.length(), nullptr, 0, &e);

	PERF_ASSERT(ol > 0);
	PERF_ASSERT(e == UTF8_ERR_NONE);

	if (ol > 0 &&
		e == UTF8_ERR_NONE)
	{
		char* o = new char[ol + 1];
		memset(o, 0, ol + 1);

		utf8totitle(m_input.c_str(), m_input.length(), o, ol, nullptr);

		delete [] o;
	}
}

#if UTF8_VERSION_GUARD(1, 4, 0)
PERF_TEST_F(CaseMappingBasicLatin, CasefoldStatic)
{
	char o[1024] = { 0 };
	size_t ol = 1023;
	int32_t e;

	size_t l = utf8casefold(m_input.c_str(), m_input.length(), o, ol, &e);

	PERF_ASSERT(l > 0);
	PERF_ASSERT(e == UTF8_ERR_NONE);
}

PERF_TEST_F(CaseMappingBasicLatin, CasefoldDynamic)
{
	int32_t e;

	size_t ol = utf8casefold(m_input.c_str(), m_input.length(), nullptr, 0, &e);

	PERF_ASSERT(ol > 0);
	PERF_ASSERT(e == UTF8_ERR_NONE);

	if (ol > 0 &&
		e == UTF8_ERR_NONE)
	{
		char* o = new char[ol + 1];
		memset(o, 0, ol + 1);

		utf8casefold(m_input.c_str(), m_input.length(), o, ol, nullptr);

		delete [] o;
	}
}
#endif

class CaseMappingLatin1
	: public performance::Suite
{

public:

	virtual void setup() override
	{
		std::vector<unicode_t> codepoints;

		for (unicode_t i = 0; i <= MAX_LATIN_1; ++i)
		{
			codepoints.push_back(i);
		}

		m_input = helpers::utf8(codepoints);
	}

	std::string m_input;

};

PERF_TEST_F(CaseMappingLatin1, LowercaseStatic)
{
	char o[MAX_LATIN_1 * 4] = { 0 };
	size_t ol = MAX_LATIN_1 * 4 - 1;
	int32_t e;

	utf8tolower(m_input.c_str(), m_input.length(), o, ol, &e);
}

PERF_TEST_F(CaseMappingLatin1, LowercaseDynamic)
{
	int32_t e;

	size_t ol = utf8tolower(m_input.c_str(), m_input.length(), nullptr, 0, &e);
	if (ol > 0 &&
		e == UTF8_ERR_NONE)
	{
		char* o = new char[ol + 1];
		memset(o, 0, ol + 1);

		utf8tolower(m_input.c_str(), m_input.length(), o, ol, nullptr);

		delete [] o;
	}
}

PERF_TEST_F(CaseMappingLatin1, UppercaseStatic)
{
	char o[MAX_LATIN_1 * 4] = { 0 };
	size_t ol = MAX_LATIN_1 * 4 - 1;
	int32_t e;

	utf8toupper(m_input.c_str(), m_input.length(), o, ol, &e);
}

PERF_TEST_F(CaseMappingLatin1, UppercaseDynamic)
{
	int32_t e;

	size_t ol = utf8toupper(m_input.c_str(), m_input.length(), nullptr, 0, &e);
	if (ol > 0 &&
		e == UTF8_ERR_NONE)
	{
		char* o = new char[ol + 1];
		memset(o, 0, ol + 1);

		utf8toupper(m_input.c_str(), m_input.length(), o, ol, nullptr);

		delete [] o;
	}
}

PERF_TEST_F(CaseMappingLatin1, TitlecaseStatic)
{
	char o[MAX_LATIN_1 * 4] = { 0 };
	size_t ol = MAX_LATIN_1 * 4 - 1;
	int32_t e;

	size_t l = utf8totitle(m_input.c_str(), m_input.length(), o, ol, &e);

	PERF_ASSERT(l > 0);
	PERF_ASSERT(e == UTF8_ERR_NONE);
}

PERF_TEST_F(CaseMappingLatin1, TitlecaseDynamic)
{
	int32_t e;

	size_t ol = utf8totitle(m_input.c_str(), m_input.length(), nullptr, 0, &e);

	PERF_ASSERT(ol > 0);
	PERF_ASSERT(e == UTF8_ERR_NONE);

	if (ol > 0 &&
		e == UTF8_ERR_NONE)
	{
		char* o = new char[ol + 1];
		memset(o, 0, ol + 1);

		utf8totitle(m_input.c_str(), m_input.length(), o, ol, nullptr);

		delete [] o;
	}
}

#if UTF8_VERSION_GUARD(1, 4, 0)
PERF_TEST_F(CaseMappingLatin1, CasefoldStatic)
{
	char o[MAX_LATIN_1 * 4] = { 0 };
	size_t ol = MAX_LATIN_1 * 4 - 1;
	int32_t e;

	size_t l = utf8casefold(m_input.c_str(), m_input.length(), o, ol, &e);

	PERF_ASSERT(l > 0);
	PERF_ASSERT(e == UTF8_ERR_NONE);
}

PERF_TEST_F(CaseMappingLatin1, CasefoldDynamic)
{
	int32_t e;

	size_t ol = utf8casefold(m_input.c_str(), m_input.length(), nullptr, 0, &e);

	PERF_ASSERT(ol > 0);
	PERF_ASSERT(e == UTF8_ERR_NONE);

	if (ol > 0 &&
		e == UTF8_ERR_NONE)
	{
		char* o = new char[ol + 1];
		memset(o, 0, ol + 1);

		utf8casefold(m_input.c_str(), m_input.length(), o, ol, nullptr);

		delete [] o;
	}
}
#endif

class CaseMappingBasicMultilingualPlane
	: public performance::Suite
{

public:

	virtual void setup() override
	{
		std::vector<unicode_t> codepoints;

		for (unicode_t i = 0; i <= MAX_BASIC_MULTILINGUAL_PLANE; ++i)
		{
			if (i < SURROGATE_HIGH_START ||
				i > SURROGATE_LOW_END)
			{
				codepoints.push_back(i);
			}
		}

		m_input = helpers::utf8(codepoints);
	}

	std::string m_input;

};

PERF_TEST_F(CaseMappingBasicMultilingualPlane, LowercaseStatic)
{
	char o[MAX_BASIC_MULTILINGUAL_PLANE * 4] = { 0 };
	size_t ol = MAX_BASIC_MULTILINGUAL_PLANE * 4 - 1;
	int32_t e;

	size_t l = utf8tolower(m_input.c_str(), m_input.length(), o, ol, &e);

	PERF_ASSERT(l > 0);
	PERF_ASSERT(e == UTF8_ERR_NONE);
}

PERF_TEST_F(CaseMappingBasicMultilingualPlane, LowercaseDynamic)
{
	int32_t e;

	size_t ol = utf8tolower(m_input.c_str(), m_input.length(), nullptr, 0, &e);

	PERF_ASSERT(ol > 0);
	PERF_ASSERT(e == UTF8_ERR_NONE);

	if (ol > 0 &&
		e == UTF8_ERR_NONE)
	{
		char* o = new char[ol + 1];
		memset(o, 0, ol + 1);

		utf8tolower(m_input.c_str(), m_input.length(), o, ol, nullptr);

		delete [] o;
	}
}

PERF_TEST_F(CaseMappingBasicMultilingualPlane, UppercaseStatic)
{
	char o[MAX_BASIC_MULTILINGUAL_PLANE * 4] = { 0 };
	size_t ol = MAX_BASIC_MULTILINGUAL_PLANE * 4 - 1;
	int32_t e;

	size_t l = utf8toupper(m_input.c_str(), m_input.length(), o, ol, &e);

	PERF_ASSERT(l > 0);
	PERF_ASSERT(e == UTF8_ERR_NONE);
}

PERF_TEST_F(CaseMappingBasicMultilingualPlane, UppercaseDynamic)
{
	int32_t e;

	size_t ol = utf8toupper(m_input.c_str(), m_input.length(), nullptr, 0, &e);

	PERF_ASSERT(ol > 0);
	PERF_ASSERT(e == UTF8_ERR_NONE);

	if (ol > 0 &&
		e == UTF8_ERR_NONE)
	{
		char* o = new char[ol + 1];
		memset(o, 0, ol + 1);

		utf8toupper(m_input.c_str(), m_input.length(), o, ol, nullptr);

		delete [] o;
	}
}

PERF_TEST_F(CaseMappingBasicMultilingualPlane, TitlecaseStatic)
{
	char o[MAX_BASIC_MULTILINGUAL_PLANE * 4] = { 0 };
	size_t ol = MAX_BASIC_MULTILINGUAL_PLANE * 4 - 1;
	int32_t e;

	size_t l = utf8totitle(m_input.c_str(), m_input.length(), o, ol, &e);

	PERF_ASSERT(l > 0);
	PERF_ASSERT(e == UTF8_ERR_NONE);
}

PERF_TEST_F(CaseMappingBasicMultilingualPlane, TitlecaseDynamic)
{
	int32_t e;

	size_t ol = utf8totitle(m_input.c_str(), m_input.length(), nullptr, 0, &e);

	PERF_ASSERT(ol > 0);
	PERF_ASSERT(e == UTF8_ERR_NONE);

	if (ol > 0 &&
		e == UTF8_ERR_NONE)
	{
		char* o = new char[ol + 1];
		memset(o, 0, ol + 1);

		utf8totitle(m_input.c_str(), m_input.length(), o, ol, nullptr);

		delete [] o;
	}
}

#if UTF8_VERSION_GUARD(1, 4, 0)
PERF_TEST_F(CaseMappingBasicMultilingualPlane, CasefoldStatic)
{
	char o[MAX_BASIC_MULTILINGUAL_PLANE * 4] = { 0 };
	size_t ol = MAX_BASIC_MULTILINGUAL_PLANE * 4 - 1;
	int32_t e;

	size_t l = utf8casefold(m_input.c_str(), m_input.length(), o, ol, &e);

	PERF_ASSERT(l > 0);
	PERF_ASSERT(e == UTF8_ERR_NONE);
}

PERF_TEST_F(CaseMappingBasicMultilingualPlane, CasefoldDynamic)
{
	int32_t e;

	size_t ol = utf8casefold(m_input.c_str(), m_input.length(), nullptr, 0, &e);

	PERF_ASSERT(ol > 0);
	PERF_ASSERT(e == UTF8_ERR_NONE);

	if (ol > 0 &&
		e == UTF8_ERR_NONE)
	{
		char* o = new char[ol + 1];
		memset(o, 0, ol + 1);

		utf8casefold(m_input.c_str(), m_input.length(), o, ol, nullptr);

		delete [] o;
	}
}
#endif