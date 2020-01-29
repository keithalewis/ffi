// lac_ffi.t.c - test ffi header
#include <assert.h>
#include "lac_ffi.h"

#define VIEW(s) s, s + sizeof(s) - 1

int test_lac_variant_parse()
{
	{
		lac_variant v = lac_variant_parse(&ffi_type_sint, VIEW("123"));
		assert (&ffi_type_sint == v.type);
		assert (123 == v.value.i);
	}
	{
		lac_variant v = lac_variant_parse(&ffi_type_sint, VIEW("123 ") - 1);
		assert (&ffi_type_sint == v.type);
		assert (123 == v.value.i);
	}
	{
		lac_variant v = lac_variant_parse(&ffi_type_sint, VIEW("123x") - 1);
		assert (&ffi_type_sint == v.type);
		assert (123 == v.value.i);
	}
	{
		lac_variant v = lac_variant_parse(&ffi_type_sint, VIEW("012"));
		assert (&ffi_type_sint == v.type);
		assert (8 + 2 == v.value.i);
	}
	{
		lac_variant v = lac_variant_parse(&ffi_type_sint, VIEW("0x12"));
		assert (&ffi_type_sint == v.type);
		assert (16 + 2 == v.value.i);
	}

	{
		lac_variant v = lac_variant_parse(&ffi_type_uint32, VIEW("123"));
		assert (&ffi_type_uint32 == v.type);
		assert (123 == v.value.u32);
	}

	{
		lac_variant v = lac_variant_parse(&ffi_type_double, VIEW("1.23"));
		assert (&ffi_type_double == v.type);
		assert (1.23 == v.value.d);
	}
}
int test_lac_ffi()
{
	test_lac_variant_parse();
}
