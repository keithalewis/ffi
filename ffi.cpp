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
#include "ffi_fun.h"
#include "ffi_dl.h"

namespace ffi {

	inline const char* skip_space(const char* b, const char* e)
	{
		while (b < e && isspace(*b))
			++b;

		return b;
	}
	
	// Get next white space delimited token
	inline token_view parse_token(const char* b, const char* e)
	{
		if (0 == *b || b == e) {
			return token_view(e,e);
		}

		bool in_string = false;
		const char* b_ = b;
		while (b_ < e && isspace(*b_)) {
			++b_;
		}

		if (b_ < e && *b_ == '"') {
			++b_; // keep leading '"' ?
			in_string = true;
		}
		const char* e_ = b_;

		bool done = false;
		while (e_ < e && !done) {
			if (!in_string) {
				if (isspace(*e_)) {
					done = true;
				}
				else {
					++e_;
				}
			}
			else { // in_string
				if ('"' == *e_) {
					done = true;
				}
				else if ('\\' == *e_) {
					++e_;
					if (e_ < e) {
						++e_;
					}
				}
				else {
					++e_;
				}
			}
		}

		if (in_string && *e_ != '"') {
			throw std::runtime_error("token_view: string quote not matched");
		}

		return token_view(b_, e_);
	}

	using line_view = std::vector<token_view>;

	inline bool empty(const token_view& p)
	{
		return p.first == p.second;
	}

	// parse a line into token views
	inline line_view parse_line(const char* b, const char* e)
	{
		line_view lv;

		token_view p = parse_token(b, e);
		while (!empty(p)) {
			lv.push_back(p);
			p = parse_token(p.second + 1, e);
		}

		return lv;
	}


}

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

int test()
{
	ffi::dl libc("libc.so.6", RTLD_LAZY);
	if (!libc) {
		perror(dlerror());
	}
	typedef int(*pf)(const char*);
	auto p = (pf)libc.sym("puts");
	p("hello world");

	ffi_cif cif;
	ffi_type* type[1];
	void* val[1];
	ffi_arg arg;

	type[0] = &ffi_type_pointer;
	const char* s;
	val[0] = &s;
	if (ffi_prep_cif(&cif, FFI_DEFAULT_ABI, 1, 
				   &ffi_type_sint, type) == FFI_OK)
	{
	  s = "Hello World!";
	  ffi_call(&cif, FFI_FN(p), &arg, val);
	  /* arg now holds the result of the call to puts */
	  
	  /* values holds a pointer to the function's arg, so to 
		 call puts() again all we need to do is change the 
		 value of s */
	  s = "This is cool!";
	  ffi_call(&cif, FFI_FN(p), &arg, val);
	}

	{
		ffi::cif cif({&ffi_type_pointer}, &ffi_type_sint);
		assert(FFI_OK == cif.prep());

		void* val[1];
		const char* s = "Hi";
		val[0] = &s;
		ffi_arg arg;
		cif.call(FFI_FN(p), &arg, &val[0]);
	}
	// example of ffi_call on a closure
	// insert missing args and call

	dlclose(libc);

	{
		ffi::dl libc("libc.so.6");
		ffi::fun<int> puts_(dlsym(libc, "puts"));
		//ffi::sym<int> puts_(libc, "puts");
		const char* s = "Hello";
		puts_(s);
		s = " there";
		puts_(s);
		puts_("you");
	}
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

std::map<std::string, ffi::thunk> ffi_dictionary;
ffi::stack ffi_stack;

int main(int ac, char* av[])
{
	test_parse_token();
	test_cif();
	test();

	ffi::dl libc("libc.so.6");
	auto puts_ = dlsym(libc, "puts");
	ffi_dictionary["puts"] = ffi::thunk(ffi::cif({&ffi_type_pointer}, &ffi_type_sint), puts_);

	std::string line;
	while (std::getline(std::cin, line)) {
		std::cout << ">" << line << "<" << std::endl;

		ffi::line_view v = ffi::parse_line(line.c_str(), line.c_str() + line.length());

		auto key = v.back();
		auto ent = ffi_dictionary.find(std::string(key.first, key.second));
		if (ent == ffi_dictionary.end()) {
			std::cerr << "entry not found: " << ent->first << std::endl;
			return 1;
		}
		const auto& t = ent->second;

		for (size_t i = 0; i < t.nargs; ++i) {
			ffi_stack.push(ffi::parse(v[i], t.arg_types[i]));
		}

		int r;
		void* ret = &r;
		t.call(ret, ffi_stack.address());

	}

	return 0;
}
