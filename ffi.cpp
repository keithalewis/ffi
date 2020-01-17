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
	using type = std::vector<ffi_type*>;
	using value = std::vector<std::unique_ptr<void*>>;

	std::variant<

	class stack {
		ffi::type type;
		ffi::value value;
	};

	std::map<std::string,std::pair<ffi_type*,std::vector<ffi_type*>>> sig = {
		{"puts", {&ffi_type_sint, {{&ffi_type_pointer}}}}
	};

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

	dlclose(libc);

	return 0;
}
int test_ = test();

template<class T>
inline std::unique_ptr<void*> make_ptr(const T& t)
{
	T* tp = new T(t);
	return std::unique_ptr<void*>(tp));
}

int main(int ac, char* av[])
{
	double x = 1;
	auto xp = make_ptr(x);

	return 0;
}
