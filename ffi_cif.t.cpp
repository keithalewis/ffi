// ffi_cif.t.cpp - test ffi::cif
#include <cassert>
#include "ffi_cif.h"

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
int test_cif_ = test_cif();
