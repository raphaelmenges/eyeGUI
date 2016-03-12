#include "performance-base.hpp"

class BigCaseMapping
	: public performance::Suite
{

public:

	virtual void setup() override
	{
		m_file.open("testdata/big.txt", std::ios_base::in);
		
		std::stringstream ss;
		ss << m_file.rdbuf();
		
		m_contents = ss.str();

		m_file.close();
	}

	std::fstream m_file;
	std::string m_contents;

};

PERF_TEST_F(BigCaseMapping, Uppercase)
{
	int32_t e;

	size_t ol = utf8toupper(m_contents.c_str(), m_contents.length(), nullptr, 0, &e);

	PERF_ASSERT(ol > 0);
	PERF_ASSERT(e == UTF8_ERR_NONE);

	if (ol > 0 &&
		e == UTF8_ERR_NONE)
	{
		char* o = new char[ol + 1];
		memset(o, 0, ol + 1);

		utf8toupper(m_contents.c_str(), m_contents.length(), o, ol, nullptr);

		delete [] o;
	}
}

PERF_TEST_F(BigCaseMapping, Lowercase)
{
	int32_t e;

	size_t ol = utf8tolower(m_contents.c_str(), m_contents.length(), nullptr, 0, &e);

	PERF_ASSERT(ol > 0);
	PERF_ASSERT(e == UTF8_ERR_NONE);

	if (ol > 0 &&
		e == UTF8_ERR_NONE)
	{
		char* o = new char[ol + 1];
		memset(o, 0, ol + 1);

		utf8tolower(m_contents.c_str(), m_contents.length(), o, ol, nullptr);

		delete [] o;
	}
}

PERF_TEST_F(BigCaseMapping, Titlecase)
{
	int32_t e;

	size_t ol = utf8totitle(m_contents.c_str(), m_contents.length(), nullptr, 0, &e);

	PERF_ASSERT(ol > 0);
	PERF_ASSERT(e == UTF8_ERR_NONE);

	if (ol > 0 &&
		e == UTF8_ERR_NONE)
	{
		char* o = new char[ol + 1];
		memset(o, 0, ol + 1);

		utf8totitle(m_contents.c_str(), m_contents.length(), o, ol, nullptr);

		delete [] o;
	}
}

#if UTF8_VERSION_GUARD(1, 4, 0)
PERF_TEST_F(BigCaseMapping, Casefold)
{
	int32_t e;

	size_t ol = utf8casefold(m_contents.c_str(), m_contents.length(), nullptr, 0, &e);

	PERF_ASSERT(ol > 0);
	PERF_ASSERT(e == UTF8_ERR_NONE);

	if (ol > 0 &&
		e == UTF8_ERR_NONE)
	{
		char* o = new char[ol + 1];
		memset(o, 0, ol + 1);

		utf8casefold(m_contents.c_str(), m_contents.length(), o, ol, nullptr);

		delete [] o;
	}
}
#endif