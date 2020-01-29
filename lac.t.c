// lac.t.cpp
#include <math.h>
#include "lac_dbm.h"
#include "lac_ffi.h"
#include "lac_parse.h"

int test_lac()
{
	ffi_type* pt[1];
	pt[0] = &ffi_type_double;
	lac_cif* cif = lac_cif_alloc_types(&ffi_type_double, 1, pt);
	assert (cif);
	cif->func = (void*)cos;
	double r, a = 0;

	void* pa[1];
	pa[0] = &a;
	ffi_call((ffi_cif*)cif, FFI_FN(cif->func), (void*)cif->cif.rtype, (void**)cif->cif.arg_types);

	lac_cif_free(cif);

	return 0;
}

int main()
{
	int test_lac_dbm();
	int test_lac_ffi();
	int test_lac_parse();

	test_lac_dbm();
	test_lac_ffi();
	test_lac_parse();

	test_lac();

	return 0;
}
