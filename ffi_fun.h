// ffi_fun.h - FFI function class
#pragma once
#include <vector>
#include "ffi_traits.h"

namespace ffi {

	// C interface
	struct cif : public ffi_cif {
		cif()
		{
			//abi = 0;
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
			//!!!check
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
		virtual ~cif()
		{
			delete [] arg_types;
		}

		//auto operator<=>(const cif&) = default;

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
		bool operator<(const cif& cif_) const
		{
			if (abi < cif_.abi)
				return true;

			if (rtype < cif_.rtype)
				return true;

			return std::lexicographical_compare(arg_types, arg_types + nargs,
				                                cif_.arg_types, cif_.arg_types + cif_.nargs);
		}
		/*
		bool operator!=(const cif& cif_) const
		{
			return !operator==(cif_);
		}
		*/

		ffi_status prep()
		{
			return ffi_prep_cif(this, abi, nargs, rtype, arg_types);
		}
		void call(void(*f)(void), void* ret, void** vals) const
		{
			ffi_call(const_cast<cif*>(this), f, ret, vals);
		}
		template<class R>
		R call(void(*f)(void), void** vals) const
		{
			R r;
			void* ret = &r;

			ffi_call(const_cast<cif*>(this), f, ret, vals);

			return r;
		}
	};

	class thunk : public cif {
		void* f;
	public:
		thunk()
			: f(nullptr)
		{ }
		thunk(const ffi::cif cif, void* f)
			: ffi::cif(cif), f(f)
		{ }
		/*
		bool operator==(const thunk& t) const
		{
			return f == t.f && cif == t.cif;
		}
		bool operator<(const thunk& t) const
		{
			if (f < t.f)
				return true;

			return cif < t.cif;
		}
		*/
		void call(void* ret, void** vals) const
		{
			cif::call(FFI_FN(f), ret, vals);
		}
	};

	// function object using symbol and cif
	template<class R> // return type
	class fun {
		void* fun_;
		mutable cif cif_;
		ffi_abi abi;
	public:
		typedef R return_type;
		// use operator() to create cif
		fun(void* fun = nullptr, ffi_abi abi = FFI_DEFAULT_ABI)
			: fun_(fun), abi(abi)
		{
			assert (!cif_);
		}
		fun(void* fun, const cif& cif)
			: fun_(fun), cif_(cif), abi(cif_.abi)
		{ }
		fun(const fun&) = default;
		fun& operator=(const fun&) = default;
		~fun()
		{ }

		void call(void* ret, void** vals) const
		{
			cif_.call(FFI_FN(fun_), ret, vals);
		}
		R call(void** vals) const
		{
			R r;
			void* ret = &r;

			call(ret, vals);

			return r;
		}
		// runtime type checked function call
		template<class... V>
		R operator()(V... vals) const
		{
			if (nullptr == fun_) {
				throw std::runtime_error("ffi::fun: null function pointer");
			}

			if (!cif_) {
				cif_ = ffi::cif({ffi::type_traits<V>::type...}, ffi::type_traits<R>::type, abi);
			}

			if (sizeof...(V) != cif_.nargs) {
				throw std::runtime_error("ffi::fun: argument count mismatch");
			}

			if (!std::equal(cif_.arg_types, cif_.arg_types + cif_.nargs,
			                std::begin({ffi::type_traits<V>::type...}))) {
				throw std::runtime_error("ffi::fun: argument type mismatch");
			}

			std::vector<void*> v{&vals...};

			return call(v.data());
		}
	};

	// stack of variants and corresponding addresses
	class stack {
		std::vector<type> v;
		std::vector<const void*> a;
	public:
		stack& push(const type& t)
		{
			const auto i = v.insert(v.begin(), t);
			a.insert(a.begin(), ffi::address(*i));

			return *this;
		}
		stack& pop(size_t n = 1)
		{
			while (n) {
				v.erase(v.begin());
				a.erase(a.begin());
				n = v.empty() ? 0 : --n;
			}

			return *this;
		}
		const type& top() const
		{
			return v.front();
		}
		void** address()
		{
			return (void**)&a[0];
		}
	};

}
