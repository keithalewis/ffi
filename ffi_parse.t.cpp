// ffi_parse.t.cpp - test parsing
#include <cassert>
#include "ffi_parse.h"

using namespace ffi;

#define endof(t) t + sizeof(t) - 1

int test_skip_space()
{
	{
		char t[] = " \t\f";
		auto e = skip_space(t, endof(t));
		assert (e == endof(t));
	}
	{
		char t[] = "abc";
		auto e = skip_space(t, endof(t));
		assert (e == t);
	}
	{
		char t[] = " \tabc";
		auto e = skip_space(t, endof(t));
		assert (*e == t[2]);
	}

	return 0;
}
int test_skip_space_ = test_skip_space();

int test_next_quote()
{
	{
		char t[] = "a";
		auto e = next_quote(t, endof(t));
		assert (e == nullptr);
	}
	{
		char t[] = "\"a";
		auto e = next_quote(t, endof(t));
		assert (e - t == 0);
	}
	{
		char t[] = "a\"a";
		auto e = next_quote(t, endof(t));
		assert (e - t == 1);
	}

	return 0;
}
int test_next_quote_ = test_next_quote();

int test_next_match()
{
	{
		char t[] = "";
		assert (*endof(t) == 0);
		auto e = next_match(t, endof(t));
		assert (e == nullptr);
	}
	{
		char t[] = "{}";
		auto e = next_match(t, endof(t));
		assert (e - t  == 2);
	}
	{
		char t[] = "{{";
		auto e = next_match(t, endof(t));
		assert (e == nullptr);
	}
	{
		char t[] = "{{";
		auto e = next_match(t, endof(t));
		assert (e == nullptr);
	}
	{
		char t[] = "{{}";
		auto e = next_match(t, endof(t));
		assert (e == nullptr);
	}
	{
		char t[] = "{{}}";
		auto e = next_match(t, endof(t));
		assert (e == endof(t));
	}

	return 0;
}
int test_next_match_ = test_next_match();
