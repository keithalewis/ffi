// lac_ffi.c
#include "lac_ffi.h"

lac_variant lac_variant_parse(ffi_type* type, const char* b, const char* e)
{
	lac_variant v;

	v.type = type;
	if (&ffi_type_sint == type) {
		char* e_;
		// check size!!!
		v.value.i = (int)strtol(b, &e_, 0);
		assert (e == e_);
	}
	else if (&ffi_type_uint32 == type) {
		char* e_;
		// check size!!!
		v.value.u32 = (uint32_t)strtoul(b, &e_, 0);
		assert (e == e_);
	}
	else if (&ffi_type_float == type) {
		char* e_;
		v.value.f = strtof(b, &e_);
		assert (e == e_);
	}
	else if (&ffi_type_double == type) {
		char* e_;
		v.value.d = strtod(b, &e_);
		assert (e == e_);
	}
	// etc!!!

	return v;
}

lac_cif* lac_cif_alloc(int n)
{
	lac_cif* p = (lac_cif*)malloc(sizeof(lac_cif) + n*sizeof(ffi_type*));

	if (!p) return p;

	p->cif.arg_types = &p->type[0];

	return p;
}

lac_cif* lac_cif_alloc_types(ffi_type* ret, int n, ffi_type** type)
{
	lac_cif* p = lac_cif_alloc(n);

	if (!p) return p;

	p->cif.rtype = ret;
	for (int i = 0; i < n; ++i) {
		p->type[i] = type[i];
	}

	p->cif.arg_types = &p->type[0];
	ffi_prep_cif((ffi_cif*)p, FFI_DEFAULT_ABI, n, ret, type);

	return p;
}

void lac_cif_free(lac_cif* p)
{
	free(p);
}



lac_variant* lac_stack_top(lac_stack* stack)
{
	return stack->data + stack->sp;
}
void lac_stack_push(lac_stack* stack, lac_variant v)
{
	stack->data[stack->sp--] = v;
}
void lac_stack_pop(lac_stack* stack)
{
	if (stack->sp < STACK_SIZE - 1) {
		++stack->sp;
	}
}
