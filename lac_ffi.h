/* lac_ffi.h - load and call C functions using libffi */
//#pragma once
#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#ifdef __cplusplus
extern "C" {
#endif
#include <ffi.h>
#ifdef __cplusplus
}
#endif

//    FFI_TYPE             type          ffi_type             union
#define FFI_TYPE_TABLE(X) \
	X(FFI_TYPE_INT,        int,         &ffi_type_sint,       i   ) \
	X(FFI_TYPE_FLOAT,      float,       &ffi_type_float,      f   ) \
	X(FFI_TYPE_DOUBLE,     double,      &ffi_type_double,     d   ) \
	X(FFI_TYPE_UINT8,      uint8_t,     &ffi_type_uint8,      u8  ) \
	X(FFI_TYPE_SINT8,      int8_t,      &ffi_type_sint8,      i8  ) \
	X(FFI_TYPE_UINT16,     uint16_t,    &ffi_type_uint16,     u16 ) \
	X(FFI_TYPE_SINT16,     int16_t,     &ffi_type_sint16,     i16 ) \
	X(FFI_TYPE_UINT32,     uint32_t,    &ffi_type_uint32,     u32 ) \
    X(FFI_TYPE_SINT32,     int32_t,     &ffi_type_sint32,     i32 ) \
	X(FFI_TYPE_UINT64,     uint64_t,    &ffi_type_uint64,     u64 ) \
	X(FFI_TYPE_SINT64,     int64_t,     &ffi_type_sint64,     i64 ) \
	X(FFI_TYPE_POINTER,    void*,       &ffi_type_pointer,    vp  ) \

	// X(FFI_TYPE_VOID,       void,        &ffi_type_void,       std::monostate, v   ) \
	// X(FFI_TYPE_LONGDOUBLE, long double, &ffi_type_longdouble, std::monostate ) \
	// X(FFI_TYPE_COMPLEX
	// X(FFI_TYPE_STRUCT,     void**,      &ffi_type_pointer,    std::monostate ) \

typedef struct {
	union {
#define X(a,b,c,d) b d;
	FFI_TYPE_TABLE(X)
#undef X
	} value;
	ffi_type* type;
} lac_variant;

lac_variant lac_variant_parse(ffi_type* type, const char* b, const char* e);

// function and argument types
typedef struct {
	void* func;
	ffi_cif cif;
	ffi_type* type[1]; // argument types
} lac_cif;

lac_cif* lac_cif_alloc(int);
lac_cif* lac_cif_alloc_types(ffi_type*, int, ffi_type**);
void lac_cif_free(lac_cif*);

// argument stack
#ifndef STACK_SIZE
#define STACK_SIZE 1024
#endif

typedef struct {
	size_t sp;
	lac_variant data[STACK_SIZE];
} lac_stack;

void lac_stack_push(lac_stack* stack, lac_variant v);
void lac_stack_pop(lac_stack* stack);
lac_variant* lac_stack_top(lac_stack* stack);
