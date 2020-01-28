// lac.c - load and call C functions
#include <cassert>
#include <ctype.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <gdbm.h>
#include "lac.h"

// use setjmp/longjmp for error handling

// gdbm specific
GDBM_FILE dictionary = 0;
inline datum make_datum(char* s)
{	
	datum d;

	d.dptr = s;
	d.dsize = (int)strlen(s);

	return d;
}
void dbm_open(void)
{
	dictionary = gdbm_open("dictionary.gdb", 0, GDBM_WRCREAT, 0664, 0);
	if (0 == dictionary) {
		fputs(gdbm_strerror(gdbm_last_errno(dictionary)), stderr);

		exit(-1);
	}
}
void dbm_close()
{
	if (dictionary) {
		gdbm_close(dictionary);
	}
}

// view into line buffer
struct token_view {
	char* b;
	char* e;
};
// copy stream into static buffer and return view
token_view get_token(FILE* fp)
{
	int c;
	static char token[1024];
	token_view t = {token, token};

	c = fgetc(fp);
	while (c != EOF && isspace(c)) {
		c = fgetc(fp);
	}

	if (EOF == c) {
		abort(); // maybe
	}

	*t.e = (char)c;
	c = fgetc(fp);
	while (EOF != c && !isspace(c)) {
		*++t.e = (char)c;
		if (t.e - t.b >= sizeof(token))
			abort();
	}

	return t;
}

void evaluate_line(FILE* fp)
{
	//token t = get_token(fp);
	// look up token in dictionary
	// get arguments based on cif
	// call
}

void load_symbol(FILE* fp)
{
	int c;
	char key[64] = "-";
	c = fgetc(fp);
	assert (c == 'l');
	strcat(key, "l");
	c = fgetc(fp);
	while (c != EOF && !isspace(c)) {
		strncat(key, (char*)&c, 1);
	}
	if (c == EOF) {
		exit(EOF);
	}
	c = skip_space(fp);
	char sym[64] = {(char)c,0};
	// get library name +xx -> lib
	// get symbol name
	// parse remaining args for cif
	// add to dictionary
}

void add_dictionary(FILE* fp)
{
	// get name
	// compile
	// add to dictionary
}

// "-lxxx" -> dlopen("libxxx.so")
void load_library(FILE* fp)
{
	int c;
	char key[64] = "-";
	char lib[1024] = "lib";

	c = fgetc(fp);
	assert (c == 'l');
	strncat(key, (char*)&c, 1);
	
	c = fgetc(fp);
	while (c != EOF && !isspace(c)) {
		strncat(key, (char*)&c, 1);
		strncat(lib, (char*)&c, 1);
		c = fgetc(fp);
	}

	if (c == EOF) {
		exit(EOF);
	}
	
	strcat(lib, ".so");
	void* h = dlopen(lib, RTLD_NOW);
	int ret = gdbm_store(dictionary, make_datum(key), datum{(char*)h, (int)sizeof(void*)}, GDBM_REPLACE);
}

void evaluate(FILE* fp)
{
	token_view t = get_token(fp);

	switch (*t.b) {
		case '-':
			load_library(fp);
			break;
		case '+':
			load_symbol(fp);
			break;
		case ':':
			add_dictionary(fp);
			break;
		default:
			evaluate_line(fp);
	}

	evaluate(fp);
}

int main(int ac, const char* av[])
{
	// process args
	FILE* fp = stdin;
	dbm_open();

	// setjmp/longjmp for error handling
	evaluate(fp);

	dbm_close();

	return 0;
}

int test_foo()
{
	{
		const char buf[] = "1.23\n";
		char* e;
		double d = strtod(buf, &e);
		assert (d == 1.23);
		assert (*e == '\n');
	}
	{
		const char buf[] = "1.23 ";
		char* e;
		double d = strtod(buf, &e);
		assert (d == 1.23);
		assert (*e == ' ');
	}

	return 0;
}
int test_foo_ = test_foo();
