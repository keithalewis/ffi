// lac.c - load and call C functions
#include <cassert>
#include <ctype.h>
#include <stdio.h>
#include "lac.h"

// next non-space character
int skip_space(FILE* fp)
{
	int c = EOF;

	while (EOF != (c = fgetc(fp))) {
		if (!isspace(c)) {
			break;
		}
	}

	return c;
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

void load_library(FILE* fp)
{
	// get library name -lxx -> libxx.so/dll
	// add to library dictionary
}

void evaluate(FILE* fp)
{
	int c;

	c = skip_space(fp);

	if (EOF == c)
		return;

	switch (c) {
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

	// setjmp/longjmp for error handling
	evaluate(fp);

	return 0;
}

int test_lacdb()
{
puts("test_lacdb");
	int ret;
	lacdb db = lacdb_alloc(); 

	char k[] = "key";
	char v[] = "val";
	lacdb_datum key = lacdb_make_datum(k);
	lacdb_datum val = lacdb_make_datum(v);
	ret = lacdb_insert(db, &key, &val);
	assert (ret);

	lacdb_datum val1 = lacdb_find(db, &key); 
	assert (val1.size == val.size);
	assert (0 == strncmp(val1.data, val.data, val.size));

	char v1[] = "val1";
	val1 = lacdb_make_datum(v1);
	ret = lacdb_replace(db, &key, &val1);
	assert (ret);

	lacdb_datum val2 = lacdb_find(db, &key); 
	assert (val2.size == val1.size);
	assert (0 == strncmp(val2.data, val1.data, val1.size));

	val2 = lacdb_find(db, &val2); 
	assert (val2.size == 0);
	assert (val2.data == nullptr);

	ret = lacdb_erase(db, &key);
	assert (ret);
	val1 = lacdb_find(db, &key); 
	assert (val1.size == 0);
	assert (val1.data == nullptr);

	lacdb_free(db);

	return 0;
}
int test_lacdb_ = test_lacdb(); 
