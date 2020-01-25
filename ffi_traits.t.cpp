// ffi_traites.t.cpp - test traits
#include <cassert>
#include "ffi_traits.h"

using namespace ffi;

template<class T>
void test_address_t(const T& t)
{
	type v{t};
	assert(*(T*)address(t) == t);
}

int test_address()
{
	test_address_t<int>(2);
	test_address_t<unsigned int>(3);
	test_address_t<float>(4.5);
	test_address_t<double>(6.7);
	std::string t{"a"};
	type v{t};
	const char* pv = (const char*)address(v);
	assert (pv[0] == 'a');
	assert (pv[1] == 0);

	return 0;
}
int test_address_ = test_address();

#define tv(s) token_view(s, s + sizeof(s) - 1)

int test_parse()
{
	{
		type t = parse(&ffi_type_void, token_view(0,0));
		assert (t == type{});
	}
	{
		//type t = parse(&ffi_type_sint, tv("123"));
	}

	return 0;
}
int test_parse_ = test_parse();
