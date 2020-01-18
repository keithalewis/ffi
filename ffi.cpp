#include <cassert>
#include <cstdio>
#include <dlfcn.h>
#include <algorithm>
#include <initializer_list>
#include <map>
#include <memory>
#include <string>
#include <valarray>
#include <variant>
#include <vector>
extern "C"
{
#include <ffi.h>
}

namespace ffi {

	template<class T>
	struct type_traits { static ffi_type* type; };

	// pointers
	template<class T> struct type_traits<T[]>
		{ static constexpr ffi_type* type = &ffi_type_pointer; };
	template<class T, size_t N> struct type_traits<T[N]>
		{ static constexpr ffi_type* type = &ffi_type_pointer; };
	template<class T> struct type_traits<T*>
		{ static constexpr ffi_type* type = &ffi_type_pointer; };
	// primitive types
	template<> struct type_traits<void>
		{ static constexpr ffi_type* type = &ffi_type_void; }; // only for return types
	template<> struct type_traits<int>
		{ static constexpr ffi_type* type = &ffi_type_sint; };
	template<> struct type_traits<unsigned int>
		{ static constexpr ffi_type* type = &ffi_type_uint; };
	template<> struct type_traits<float>
		{ static constexpr ffi_type* type = &ffi_type_float; };
	template<> struct type_traits<double>
		{ static constexpr ffi_type* type = &ffi_type_double; };
	template<> struct type_traits<char>
		{ static constexpr ffi_type* type = std::is_signed<char>::value ? &ffi_type_schar : &ffi_type_uchar; };
	template<> struct type_traits<unsigned char>
		{ static constexpr ffi_type* type = &ffi_type_uchar; };
	template<> struct type_traits<short>
		{ static constexpr ffi_type* type = &ffi_type_sshort; };
	template<> struct type_traits<unsigned short>
		{ static constexpr ffi_type* type = &ffi_type_ushort; };
	template<> struct type_traits<long>
		{ static constexpr ffi_type* type = &ffi_type_slong; };
	template<> struct type_traits<unsigned long>
		{ static constexpr ffi_type* type = &ffi_type_ulong; };

	using type = std::variant<int, unsigned int, double, char, void*>;

	// C interface
	struct cif : public ffi_cif {
		cif()
		{
			rtype = nullptr;
			nargs = 0;
			arg_types = nullptr;
		}
		cif(std::initializer_list<ffi_type*> arg_types, // = {}
            ffi_type* rtype = &ffi_type_sint,
            ffi_abi abi = FFI_DEFAULT_ABI)
		{
			ffi_cif::rtype = rtype;
			ffi_cif::abi = abi;
			nargs = arg_types.size();
			ffi_cif::arg_types = nargs ? new ffi_type*[nargs] : nullptr;
			std::copy(arg_types.begin(), arg_types.end(), ffi_cif::arg_types);

			if (FFI_OK != prep()) {
				delete [] ffi_cif::arg_types;

				throw std::runtime_error("ffi::cif constructor ffi_prep_cif failed");
			}
		}
		cif(const cif& cif_)
			: ffi_cif(cif_)
		{
			arg_types = new ffi_type*[nargs];
			std::copy(cif_.arg_types, cif_.arg_types + nargs, arg_types);
		}
		cif(cif&& cif_)
			: ffi_cif(cif_)
		{
			cif_.rtype = nullptr;
			cif_.nargs = 0;
			cif_.arg_types = nullptr;
		}
		cif& operator=(const cif& cif_)
		{
			if (this != &cif_) {
				delete [] arg_types;
				ffi_cif::operator=(cif_);
				arg_types = new ffi_type*[nargs];
				std::copy(cif_.arg_types, cif_.arg_types + nargs, arg_types);
			}

			return *this;
		}
		cif& operator=(cif&& cif_)
		{
			delete [] arg_types;

			ffi_cif::operator=(cif_);
			cif_.rtype = nullptr;
			cif_.nargs = 0;
			cif_.arg_types = nullptr;

			return *this;
		}
		~cif()
		{
			delete [] arg_types;
		}

		operator bool() const
		{
			return nullptr != rtype;
		}

		bool operator==(const cif& cif_) const
		{
			if (abi != cif_.abi)
				return false;

			if (rtype != cif_.rtype)
				return false;

			if (nargs != cif_.nargs)
				return false;

			return std::equal(arg_types, arg_types + nargs, cif_.arg_types);
		}
		bool operator!=(const cif& cif_) const
		{
			return !operator==(cif_);
		}

		ffi_status prep()
		{
			return ffi_prep_cif(this, abi, nargs, rtype, arg_types);
		}
		void call(void(*f)(void), void* ret, void** vals) const
		{
			ffi_call(const_cast<cif*>(this), f, ret, vals);
		}
	};

	template<class R> // return type
	class fun {
		cif cif_;
		void* fun_;
	public:
		fun(const cif& cif, void* fun)
			: cif_(cif), fun_(fun)
		{ }
		// runtime type checked function call
		template<class... A>
		R operator()(A... as)
		{
			if (sizeof...(A) != cif_.nargs) {
				throw std::runtime_error("ffi::fun: argument count mismatch");
			}

			if (!std::equal(cif_.arg_types, cif_.arg_types + cif_.nargs, std::begin({ffi::type_traits<A>::type...}))) {
				throw std::runtime_error("ffi::fun: argument type mismatch");
			}

			R r;
			void* arg = &r;
			std::vector<void*> v{&as...};

			ffi_call(&cif_, FFI_FN(fun_), r, v.data());

			return r;
		}
	};

	class dl {
		void* lib;
	public:
		dl(const char* lib, int flags = RTLD_LAZY)
			: lib(dlopen(lib, flags))
		{ 
			if (nullptr == lib) {
				std::runtime_error(dlerror());
			}
		}
		dl(const dl&) = delete;
		dl& operator=(const dl&) = delete;
		~dl()
		{
			dlclose(lib);
		}
		operator void*() 
		{
			return lib;
		}
	};

	template<class R> // return type of symbol
	class sym {
		ffi_abi abi;
		void* sym_;
		ffi_type* result_type;
		ffi::cif cif;
	public:
		sym(void* lib, const char* symbol, ffi_abi abi = FFI_DEFAULT_ABI)
			: result_type(ffi::type_traits<R>::type), sym_(dlsym(lib, symbol))
		{
			if (nullptr == sym_) {
				throw std::runtime_error(dlerror());
			}
		}
		sym(const sym&) = default;
		sym& operator=(const sym&) = default;
		~sym()
		{ }

		operator void*()
		{
			return sym_;
		}

		template<class... A>
		R operator()(A... as)
		{
			if (!cif) {
				cif = ffi::cif({ffi::type_traits<A>::type...}, result_type, abi);
			}

			R r;
			void* arg = &r;
			std::vector<void*> v{&as...};

			cif.call(FFI_FN(sym_), &arg, v.data());

			return r;
		}
	};
	
	// curried functions
	// cif, f

//	template<class R, class... A>

	/*
	std::map<std::string,ffi_cif> sig = {
		{"puts", {&ffi_type_sint, {{&ffi_type_pointer}}}}
	};
	*/

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
		ffi::sym<int> puts_(libc, "puts");
		const char* s = "Hello";
		puts_(s);
		s = " there";
		puts_(s);
		puts_("you");
	}

	return 0;
}

int main(int ac, char* av[])
{
	test_cif();
	test();

	return 0;
}
