// ffi_parse.t.cpp - test parsing
#include <cassert>
#include <cstring>
#include <unistd.h>
#include <algorithm>
#include "ffi_parse.h"

using namespace ffi;

#define endof(t) ((t + sizeof(t)) - 1)

void print(const token_view& v)
{
	write(1, ">", 1);
	write(1, v.first, v.second - v.first);
	write(1, "<\n", 2);
}

#define THROWS(e) bool thrown = false; try { e; } catch (const std::exception&) { thrown = true; } assert (thrown)

int test_skip_space()
{
	{
		char t[] = "";
		assert (t == endof(t));
		assert (*endof(t) == 0);
	}
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
		assert (e == t + 2);
	}

	return 0;
}
int test_skip_space_ = test_skip_space();

int test_next_quote()
{
	{
		char t[] = "a";
		THROWS(next_quote(t, endof(t)));
	}
	{
		char t[] = "\"a";
		THROWS(next_quote(t, endof(t)));
	}
	{
		char t[] = "\" a";
		THROWS(next_quote(t, endof(t)));
	}
	{
		char t[] = "a\"a";
		THROWS(next_quote(t, endof(t)));
	}
	{
		char t[] = "a\" a\"";
		THROWS(next_quote(t, endof(t)));
	}
	{
		char t[] = "\"a \"";
		auto e = next_quote(t, endof(t));
		assert (t + 4 == e);
	}

	return 0;
}
int test_next_quote_ = test_next_quote();

int test_next_match()
{
	{
		char t[] = "{}";
		auto e = next_match(t, endof(t));
		assert (e == t + 2);
	}
	{
		char t[] = "{{";
		THROWS(next_match(t, endof(t)));
	}
	{
		char t[] = "{{}";
		THROWS(next_match(t, endof(t)));
	}
	{
		char t[] = "{{}}";
		auto e = next_match(t, endof(t));
		assert (e == endof(t));
	}

	return 0;
}
int test_next_match_ = test_next_match();

int test_parse_token()
{
	{
		char s[] = "";
		assert (s == endof(s));
		auto t = parse_token(s, endof(s));
		assert (empty(t));
	}
	{
		char s[] = "a";
		auto t = parse_token(s, endof(s));
		assert (equal(t, "a"));
		t = parse_token(t.second, endof(s));
		assert (empty(t));
	}
	{
		char s[] = "a b";
		auto t = parse_token(s, endof(s));
		assert (equal(t, "a"));
		t = parse_token(t.second, endof(s));
		assert (equal(t, "b"));
		t = parse_token(t.second, endof(s));
		assert (empty(t));
	}
	{
		char s[] = "a b ";
		auto t = parse_token(s, endof(s));
		assert (equal(t, "a"));
		t = parse_token(t.second, endof(s));
		assert (equal(t, "b"));
		t = parse_token(t.second, endof(s));
		assert (empty(t));
	}
	{
		char s[] = "\ta \fb ";
		auto t = parse_token(s, endof(s));
		assert (equal(t, "a"));
		t = parse_token(t.second, endof(s));
		assert (equal(t, "b"));
		t = parse_token(t.second, endof(s));
		assert (empty(t));
	}
	{
		char s[] = "\ta \f\"b\" ";
		auto t = parse_token(s, endof(s));
		assert (equal(t, "a"));
		t = parse_token(t.second, endof(s));
		assert (equal(t, "\"b\""));
		t = parse_token(t.second, endof(s));
		assert (empty(t));
	}
	{
		char s[] = "a \\\"b\\\" {c}";
		auto t = parse_token(s, endof(s));
		assert (equal(t, "a"));
		t = parse_token(t.second, endof(s));
		assert (equal(t, "\\\"b\\\""));
		t = parse_token(t.second, endof(s));
		assert (equal(t, "{c}"));
		t = parse_token(t.second, endof(s));
		assert (empty(t));
	}
	{
		char s[] = "a \\\"b\\\" {c {d} e}";
		auto t = parse_token(s, endof(s));
		assert (equal(t, "a"));
		t = parse_token(t.second, endof(s));
		assert (equal(t, "\\\"b\\\""));
		t = parse_token(t.second, endof(s));
		assert (equal(t, "{c {d} e}"));
		t = parse_token(t.second, endof(s));
		assert (empty(t));
	}
	{
		char s[] = "a \"b b\" {c {d} e}";
		auto t = parse_token(s, endof(s));
		assert (equal(t, "a"));
		t = parse_token(t.second, endof(s));
		assert (equal(t, "\"b b\""));
		t = parse_token(t.second + 1, endof(s));
		assert (equal(t, "{c {d} e}"));
		t = parse_token(t.second, endof(s));
		assert (empty(t));
	}

	return 0;
}
int test_parse_token_ = test_parse_token();

int test_parse_line()
{
	{
		char s[] = "a \"b c\" {d}";
		auto v = parse_line(s, endof(s));
		assert (v.size() == 3);
		assert (equal(v[0], "a"));
		assert (equal(v[1], "\"b c\""));
		assert (equal(v[2], "{d}"));
	}
	{
		char s[] = "a \"b c\"{d}";
		auto v = parse_line(s, endof(s));
		assert (v.size() == 3);
		assert (equal(v[0], "a"));
		assert (equal(v[1], "\"b c\""));
	}

	return 0;
}
int test_parse_line_ = test_parse_line();
