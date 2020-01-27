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
std::map<std::string, std::pair<void*,ffi::cif>> dictionary;
ffi::stack stack_;

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

ffi_type* get_type(const token_view& v)
{
	auto i = type_map.find(make_string(v));
	if (i == type_map.end()) {
		throw std::runtime_error("get_type: type not found");
	}

	return i->second;
}

void load_library(const token_list& v)
{
	// use -lxxx for key. all symbols go in the same dictionary
	auto i = library.find(make_string(v[0]));
	if (i == library.end()) {
		std::string lib = lib_name(v[0]);
		ffi::dl dl(lib.c_str()); // LAZY???
		//assert (dl);
		auto ib = library.insert(std::pair(lib, dl));
		assert (ib.second);
		i = ib.first;
	}
	auto func = make_string(v[1]);
	void* sym = i->second.sym(func.c_str());
	if (nullptr == sym) {
		throw std::runtime_error("load_library: symbol not found");
	}
	ffi_type* ret = get_type(v[2]);
	std::vector<ffi_type*> arg;
	for (size_t i = 3; i < v.size(); ++i) {
		arg.push_back(get_type(v[i]));
	}
	ffi::cif cif(arg, ret);
	dictionary[func] = std::pair(sym, cif);
}

void evaluate(const char* b, const char* e)
{
	token_list v = ffi::parse_line(b, e);

	for (const auto& t : v) {
		std::cout << ">" << std::string_view(t.first, t.second - t.first) << "<" << std::endl;
	}

	if (0 == strncmp(v[0].first, "-l", 2)) {
		load_library(v);
	}
	else {
		const auto d = dictionary.find(make_string(v[0]));
		if (d != dictionary.end()) {
			void* f = d->second.first;
			const ffi::cif& cif = d->second.second;
			v.erase(v.begin());
			for (int i = v.size() - 1; i >= 0; --i) {
				//!! process specials
				stack_.push(parse(cif.arg_types[i], v[i]));
			}
			type r = make_type(cif.rtype);
			cif.call(f, address(r), stack_.address());
			stack_.pop(cif.nargs);
			stack_.push(r);
		}
	}
	// else if v[0] in dictionary
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
