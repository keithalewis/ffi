#include <cassert>
#include <cstdio>
#include <dlfcn.h>
#include <map>
#include <memory>
#include <string>
#include <vector>
extern "C"
{
#include <ffi.h>
}

namespace ffi {

	class cif : public ffi_cif {
		std::vector<ffi_type*> type;
	public:
		cif(ffi_type* rtype = &ffi_type_sint, ffi_abi abi = FFI_DEFAULT_ABI)
		{
			ffi_cif::abi = abi;
			ffi_cif::rtype = rtype;
		}
		// cif(initialization_list<ffi_type*> args, ffi_type* rtype = &ffi_type_sint, ffi_abi abi = FFI_DEFAULT_ABI)
		// use nullptr for missing args
		cif& push(ffi_type* arg)
		{
			type.push_back(arg);
			++ffi_cif::nargs;
			//??? ffi_cif::arg_types = &type[0];

			return *this;
		}
		ffi_status prep()
		{
			ffi_cif::arg_types = &type[0];

			return ffi_prep_cif(this, abi, nargs, rtype, &type[0]);
		}
		void call(void(*f)(void), ffi_arg* ret, void** vals)
		{
			ffi_call(this, f, ret, vals);
		}
	};

	/*
	std::map<std::string,ffi_cif> sig = {
		{"puts", {&ffi_type_sint, {{&ffi_type_pointer}}}}
	};
	*/

}
int test()
{
	void* libc = dlopen("libc.so.6", RTLD_LAZY);
	if (!libc) {
		perror(dlerror());
	}
	typedef int(*pf)(const char*);
	auto p = (pf)dlsym(libc, "puts");
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
		ffi::cif cif(&ffi_type_sint);
		cif.push(&ffi_type_pointer);
		assert(FFI_OK == cif.prep());

		void* val[1];
		const char* s = "Hi";
		val[0] = &s;
		ffi_arg arg;
		cif.call(FFI_FN(p), &arg, &val[0]);
	}

	dlclose(libc);

	return 0;
}

int main(int ac, char* av[])
{
	test();

	return 0;
}
