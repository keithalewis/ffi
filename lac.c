// lac.c - load and call C functions
#include <ctype.h>
#include <stdio.h>

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
