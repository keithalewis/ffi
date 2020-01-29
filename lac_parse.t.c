// lac_parse.t.cpp - test parsing
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "lac_parse.h"

int test_lac_parse()
{
	token_view t;
	FILE* fp = tmpfile();

	{
		fputs("a b c", fp);
		rewind(fp);

		t = get_token(fp);
		assert (0 == strcmp(t.b, "a"));
		t = get_token(fp);
		assert (0 == strcmp(t.b, "b"));
		t = get_token(fp);
		assert (0 == strcmp(t.b, "c"));
		t = get_token(fp);
		assert (token_view_empty(t));
	}
	{
		rewind(fp);
		fputs("a\tb c", fp);
		rewind(fp);

		t = get_token(fp);
		assert (0 == strcmp(t.b, "a"));
		t = get_token(fp);
		assert (0 == strcmp(t.b, "b"));
		t = get_token(fp);
		assert (0 == strcmp(t.b, "c"));
		t = get_token(fp);
		assert (token_view_empty(t));
	}


	fclose(fp);

	return 0;
}
