// ffi_traits.ch 
#pragma once
extern "C"
{
#include <ffi.h>
}
#include <cerrno>
#include <charconv>
#include <cstdint>
#include <stdexcept>
//#include <string_view>
#include <variant>

//    FFI_TYPE             type          ffi_type             variant
#define FFI_TYPE_TABLE(X) \
	X(FFI_TYPE_VOID,       void,        &ffi_type_void,       std::monostate ) \
	X(FFI_TYPE_INT,        int,         &ffi_type_sint,       int            ) \
	X(FFI_TYPE_FLOAT,      float,       &ffi_type_float,      float          ) \
	X(FFI_TYPE_DOUBLE,     double,      &ffi_type_double,     double         ) \
	X(FFI_TYPE_LONGDOUBLE, long double, &ffi_type_longdouble, std::monostate ) \
	X(FFI_TYPE_UINT8,      uint8_t,     &ffi_type_uint8,      uint8_t        ) \
	X(FFI_TYPE_SINT8,      int8_t,      &ffi_type_sint8,      int8_t         ) \
	X(FFI_TYPE_UINT16,     uint16_t,    &ffi_type_uint16,     uint16_t       ) \
	\
	X(FFI_TYPE_SINT16,     int16_t,     &ffi_type_sint16,     int16_t        ) \
	X(FFI_TYPE_UINT32,     uint32_t,    &ffi_type_uint32,     uint32_t       ) \
	X(FFI_TYPE_UINT64,     uint64_t,    &ffi_type_uint64,     uint64_t       ) \
	X(FFI_TYPE_SINT64,     int64_t,     &ffi_type_sint64,     int64_t        ) \
	X(FFI_TYPE_POINTER,    void*,       &ffi_type_pointer,    void*          ) \

    // X(FFI_TYPE_SINT32,     int32_t,     &ffi_type_sint32,     int32_t        ) \
	// X(FFI_TYPE_STRUCT,     void**,      &ffi_type_pointer,    std::monostate ) \
	// FFI_TYPE_COMPLEX

namespace ffi {

#define X(a,b,c,d) d,
	using type = std::variant<
		FFI_TYPE_TABLE(X)
		std::string
	>;
#undef X

	// address of variant alternative
	struct visitor {
		const void* operator()(const std::monostate& t) const
		{
			return nullptr;
		}
		const void* operator()(const std::string& t) const
		{
			return t.c_str();
		}
		template<class T>
		const void* operator()(const T& t) const
		{
			return &t;
		}
	};
	// address of current alternative
	inline const void* address(const type& t)
	{
		return std::visit(visitor{}, t);
	}

	template<class T>
	struct type_traits { static ffi_type* type; };

	// pointers
	template<class T> struct type_traits<T[]>
		{ static constexpr ffi_type* type = &ffi_type_pointer; };
	template<class T, size_t N> struct type_traits<T[N]>
		{ static constexpr ffi_type* type = &ffi_type_pointer; };
	template<class T> struct type_traits<T*>
		{ static constexpr ffi_type* type = &ffi_type_pointer; };
	// char
	template<> struct type_traits<char>
		{ static constexpr ffi_type* type = std::is_signed<char>::value ? &ffi_type_schar : &ffi_type_uchar; };
#define X(a,b,c,d) template<> struct type_traits<b> { static constexpr ffi_type* type = c; };
	FFI_TYPE_TABLE(X)
#undef X

	template<ffi_type* T>
	struct traits_type { };

#define X(a,b,c,d) template<> struct traits_type<c> { typedef b type; };
	FFI_TYPE_TABLE(X)
#undef X

	template<class T>
	inline T parse(const char* b, const char* e)
	{
		T t;

//??? use radix 0 for integral types???
		std::from_chars_result result = std::from_chars(b, e, t);
		if (result.ptr != e) {
			throw std::runtime_error("ffi::parse: incomplete match");
		}

		return t;
	}

	using token_view = std::pair<const char*, const char*>;

	template<class T>
	inline T parse(const token_view& p)
	{
		const auto [b, e] = p;

		return parse<T>(b, e);
	}

	//!!!use sting_view instead of token_view
	inline const std::string_view make_view(const token_view& v)
	{
		return std::string_view(v.first, v.second - v.first);
	}

	// use {code} to push pointers
	inline type parse(const token_view& p, ffi_type* t)
	{
		if (t == &ffi_type_pointer) {
			// if first char is { then execute code
			return type(std::string(p.first, p.second));
		}

#define X(a,b,c,d) if (t == c) return type(parse<b>(p));
		//FFI_TYPE_TABLE(X);
#undef X

		return type{}; // first element is std::monostate
	}

}
