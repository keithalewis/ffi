// lac_parse.h - parsing functions
#pragma once
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>

// view into line buffer
typedef struct token_view_ {
	char* b;
	char* e;
} token_view;

// b == e
bool token_view_empty(const token_view t);

// return b if e is null
char* token_view_error(const token_view t);

// copy stream into static buffer and return view
token_view get_token(FILE* fp);

extern const int test_lac_parse_;
