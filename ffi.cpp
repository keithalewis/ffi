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
#include "ffi_cif.h"
#include "ffi_dl.h"


using namespace ffi;

std::map<std::string, ffi::dl> library; // library map;

// convert -lxx to libxx.so
std::string lib_name(const token_view& v)
{
	assert(v.first[0] == '-');
	assert(v.first[1] == 'l');

	std::string lib("lib");
	lib.append(v.first + 2, v.second);
	lib.append(".so"); //!!! linux

	return lib;
}

void load_library(const token_list& v)
{
	assert(v.size() > 2);
	std::string lib = lib_name(v[0]);
	auto i = library.find(lib);
	if (i == library.end()) {
		ffi::dl dl(make_string(v[1]).c_str());
		auto ib = library.insert(std::pair(lib, dl));
		assert (ib.second);
		i = ib.first;
	}

}

void evaluate(const char* b, const char* e)
{
	std::vector<token_view> v = ffi::parse_line(b, e);

	for (const auto& t : v) {
		std::cout << ">" << std::string_view(t.first, t.second - t.first) << "<" << std::endl;
	}

	if (v.size() > 0) {
		if (0 == strncmp(v[0].first, "-l", 2)) {
			load_library(v);
		}
	}
}

int main(int ac, char* av[])
{
	// read from stdin
	int lineno = 0;

	try {
		std::string line;
		while (std::getline(std::cin, line)) {
			++lineno;
			std::cout << ">" << line << "<" << std::endl;

			evaluate(&line[0], &line[0] + line.length());
		}
	}
	catch (const std::exception& ex) {
		std::cerr << "exception: " << ex.what() << std::endl;
	}

	return 0;
}
