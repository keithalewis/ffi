// lac_parse.h - parsing functions
#include "lac_parse.h"

bool token_view_empty(const token_view t)
{
	return t.b == t.e;
}
// return information if error
char* token_view_error(const token_view t)
{
	return t.e == 0 ? t.b : 0;
}
// copy stream into static buffer and return view
token_view get_token(FILE* fp)
{
	int c;
	static char token[1024];
	token_view t = {token, token};

	c = fgetc(fp);
	// skip space
	while (c != EOF && isspace(c)) {
		c = fgetc(fp);
	}

	while (EOF != c && !isspace(c)) {
		*t.e++ = (char)c;
		if (t.e - t.b >= sizeof(token)) {
			t.e = 0;

			return t;
		}
		c = getc(fp);
	}
	*t.e = 0; // null terminate

	return t;
}
