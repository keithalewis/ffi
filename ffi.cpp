#include <cassert>
#include <cstdio>
#include <cstring>
#include <algorithm>
#include <deque>
#include <iostream>
#include <initializer_list>
#include <map>
#include <memory>
#include <stack>
#include <string>
#include <valarray>
#include <variant>
#include <vector>
#include "ffi_parse.h"
#include "ffi_fun.h"
#include "ffi_dl.h"

int test_cif()
{
	{
		ffi::cif c;
		ffi::cif c2(c);
		assert (c == c2);
		c = c2;
		assert (!(c2 != c));
		c2 = std::move(c);
		assert (c.nargs == 0);
		assert (c.arg_types == nullptr);
	}
	{
		ffi::cif c({ffi::type_traits<int>::type});
		ffi::cif c2(c);
		assert (c == c2);
		c = c2;
		assert (!(c2 != c));

		assert (c.nargs == 1);
		assert (c.arg_types[0] == &ffi_type_sint);
	}
	{
		ffi_type* type = ffi::type_traits<char>::type;
		assert (type == (std::is_signed<char>::value ? &ffi_type_schar : &ffi_type_uchar));
	}

	return 0;
}

int test_dl()
{
	{
		ffi::dl libm("libm.so");
		ffi::fun<double> hypot_(dlsym(libm, "hypot"));
		assert (hypot_(3.,4.) == 5);
	}

	return 0;
}

/*
class stack {
	std::vector<type> s;
public:
	stack()
	{}
};

class thunk;

thunk read_thunk()
{
	int c;
	while (EOF != (c = getc()) {
	}
}
*/

int test_parse_token()
{
	{
		const char s[] = "";
		auto p = ffi::parse_token(s, s + sizeof s);
		assert (ffi::empty(p));
	}
	{
		const char s[] = "a b";
		auto [b,e] = ffi::parse_token(s, s + sizeof s);
		char t[] = "a";
		assert (std::equal(b, e, t, t + strlen(t)));
	}
	{
		const char s[] = " \ta b";
		auto [b,e] = ffi::parse_token(s, s + sizeof s);
		char t[] = "a";
		assert (std::equal(b, e, t, t + strlen(t)));
	}
	{
		const char s[] = "\"a b\" c";
		auto [b,e] = ffi::parse_token(s, s + sizeof s);
		assert (std::equal(b, e, "a b"));
		assert (e - b == strlen("a b"));
		auto [c,f] = ffi::parse_token(e+1, s + sizeof s);
		assert (std::equal(c, f, "c"));
		auto p = ffi::parse_token(f+1, s + sizeof s);
		assert (ffi::empty(p));
	}
	{
		const char s[] =  {'"', 'a', '\\', '"', 'b', '"'};
		auto [b,e] = ffi::parse_token(s, s + sizeof s);
		assert (std::equal(b, e, "a\\\"b"));
	}

	return 0;
}

int test_parse()
{
	using ffi::token_view;
	using ffi::type;

	type t;
	const char s[] = "data";
	t = ffi::parse(&ffi_type_void, token_view(s, s + sizeof(s)));

	return 0;
}

using namespace ffi;
std::map<std::string, ffi::thunk> ffi_dictionary;
ffi::stack ffi_stack;

int main(int ac, char* av[])
{
	// read from stdin
	int lineno = 0;

	try {
		std::string line;
		while (std::getline(std::cin, line)) {
			++lineno;
			std::cout << ">" << line << "<" << std::endl;

			std::vector<token_view> v = ffi::parse_line(line.c_str(), line.c_str() + line.length());
			for (const auto& t : v) {
				std::cout << ">" << std::string_view(t.first, t.second - t.first) << "<" << std::endl;
			}
		}
	}
	catch (const std::exception& ex) {
		std::cerr << "exception: " << ex.what() << std::endl;
	}

	return 0;
}
