// lac_dbm.t.c - test database absraction
#include <assert.h>
#include <string.h>
#include "lac_dbm.h"

#define DATUM(s) (lac_datum){s,strlen(s)}

int test_lac_dbm()
{
	int ret;
	void* foo = lac_dbm_open("foo");

	ret = lac_dbm_insert(foo, DATUM("key"), DATUM("value"));
	assert (0 == ret);
	lac_datum kv = lac_dbm_fetch(foo, DATUM("key"));
	assert (0 == strcmp(kv.data, "value"));
	kv = lac_dbm_fetch(foo, DATUM("foo"));
	assert (0 == kv.data);

	lac_dbm_close(foo);

	return 0;
}
