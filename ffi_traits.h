// ffi_traits.ch 
#pragma once
extern "C"
{
#include <ffi.h>
}
#include <cerrno>
#include <charconv>
#include <cstdint>
#include <cstring>
#include <map>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <variant>
#include <vector>

//    FFI_TYPE             type          ffi_type             variant         size
#define FFI_TYPE_TABLE(X) \
	X(FFI_TYPE_VOID,       void,        &ffi_type_void,       std::monostate ) \
	X(FFI_TYPE_INT,        int,         &ffi_type_sint,       int            ) \
	X(FFI_TYPE_FLOAT,      float,       &ffi_type_float,      float          ) \
	X(FFI_TYPE_DOUBLE,     double,      &ffi_type_double,     double         ) \
	X(FFI_TYPE_UINT8,      uint8_t,     &ffi_type_uint8,      uint8_t        ) \
	X(FFI_TYPE_SINT8,      int8_t,      &ffi_type_sint8,      int8_t         ) \
	X(FFI_TYPE_UINT16,     uint16_t,    &ffi_type_uint16,     uint16_t       ) \
	X(FFI_TYPE_SINT16,     int16_t,     &ffi_type_sint16,     int16_t        ) \
	X(FFI_TYPE_UINT32,     uint32_t,    &ffi_type_uint32,     uint32_t       ) \
	X(FFI_TYPE_UINT64,     uint64_t,    &ffi_type_uint64,     uint64_t       ) \
	X(FFI_TYPE_SINT64,     int64_t,     &ffi_type_sint64,     int64_t        ) \
	X(FFI_TYPE_POINTER,    void*,       &ffi_type_pointer,    void*          ) \

	// X(FFI_TYPE_LONGDOUBLE, long double, &ffi_type_longdouble, std::monostate ) \
	// X(FFI_TYPE_COMPLEX
    // X(FFI_TYPE_SINT32,     int32_t,     &ffi_type_sint32,     int32_t        ) \
	// X(FFI_TYPE_STRUCT,     void**,      &ffi_type_pointer,    std::monostate ) \

namespace ffi {

	// stack types
#define X(a,b,c,d) d,
	using type = std::variant<
		FFI_TYPE_TABLE(X)
		std::string
	>;
#undef X

	// map from string name to ffi_type
	inline std::map<std::string, ffi_type*> type_map = {
#define X(a,b,c,d) {std::string(#b), c},
		FFI_TYPE_TABLE(X)
#undef X
	};

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

	// type and storage size of ffi_type
	template<ffi_type* T>
	struct traits_type { static constexpr std::size_t size = 0; };
#define X(a,b,c,d) template<> struct traits_type<c> { \
	typedef b type; if constexpr (!std::is_void_v<b>) \
		static constexpr std::size_t size = 0; \
		else static constexpr std::size_t size = sizeof(b); } \
	FFI_TYPE_TABLE(X)
#undef X


		/*
	template<class T>
	inline T parse(const char* b, const char* e)
	{
		T t;

		if constexpr (std::is_void_v<T>)
			return;
		//!!!gcc workaround
		if constexpr (std::is_same<T,double>::value)
			return strtod(b, NULL); //!!!no error checking
		if constexpr (std::is_same<T,float>::value)
			return strtof(b, NULL); //!!!no error checking
		if constexpr (std::is_integral<T>::value)
			result = std::from_chars(b, e, t, 0);
		else
			result = std::from_chars(b, e, t);

		if (result.ptr != e) {
			throw std::runtime_error("ffi::parse: incomplete match");
		}

		return t;
	}

	template<class T>
	inline T parse(const token_view& p)
	{
		const auto [b, e] = p;

	template<>
	inline type from_chars<float>(const token_view& p)
	{
		
	}

	inline type parse(const ffi_type* t, const token_view& p)
	{
		const auto [b, e] = p;

		if (&ffi_type_void == t) {
			return type{};
		}
		if (&ffi_type_sint == t) {
			return from_char<int>(p);
		}

		return type{};
	}

		/*
	template<class T>
	inline T parse(const char* b, const char* e)
	{
		T t;

		if constexpr (std::is_void_v<T>)
			return;
		//!!!gcc workaround
		if constexpr (std::is_same<T,double>::value)
			return strtod(b, NULL); //!!!no error checking
		if constexpr (std::is_same<T,float>::value)
			return strtof(b, NULL); //!!!no error checking
		if constexpr (std::is_integral<T>::value)
			result = std::from_chars(b, e, t, 0);
		else
			result = std::from_chars(b, e, t);

		if (result.ptr != e) {
			throw std::runtime_error("ffi::parse: incomplete match");
		}

		return t;
	}

	template<class T>
	inline T parse(const token_view& p)
	{
		const auto [b, e] = p;

		return parse<T>(b, e);
	}
		*/

/*
#define X(a,b,c,d) if constexpr (std::is_same_v<b,double>) { if (t == c) return parse<b>(p); }
		FFI_TYPE_TABLE(X);
#undef X
*/

}
