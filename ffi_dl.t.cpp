// ffi_dl.t.cpp
#include <cassert>
#include "ffi_dl.h"

int test_dl()
{
	try {
		ffi::dl libm("libm.so");
		//assert(libm);

		double (*cos_)(double) = (double(*)(double))libm.sym("cos");
		assert (cos_(0) == 1);

		double (*sin_)(double) = (double(*)(double))libm.sym("sin");
		assert (sin_(0) == 0);
	}
	catch (const std::exception& ex) {
		puts(ex.what());	
	}

	// void* cif = dlsym(RTLD_DEFAULT, "ffi_cif_prep");

	return 0;
}
int test_dl_ = test_dl();
