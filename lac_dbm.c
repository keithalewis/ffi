// lac_dbm.c
#include <stdlib.h>
#include <string.h>
#include <gdbm.h>
#include "lac_dbm.h"

static inline datum DATUM(lac_datum d)
{
	return (datum){d.data, d.size};
}

void* lac_dbm_open(const char* name)
{
	size_t n = strlen(name);
	char* gdb = malloc(n + 5);

	strncpy(gdb, name, n);
	strncat(gdb, ".gdb", 5);
	GDBM_FILE h = gdbm_open(gdb, 0, GDBM_NEWDB, 0664, 0);

	free(gdb);

	return (void*)h;
}

int lac_dbm_close(void* dbm)
{
	return dbm ? gdbm_close((GDBM_FILE)dbm) : 0;
}

int lac_dbm_replace(void* dbm, lac_datum key, lac_datum val)
{
	return gdbm_store((GDBM_FILE)dbm, DATUM(key), DATUM(val), GDBM_REPLACE);
}
int lac_dbm_insert (void* dbm, lac_datum key, lac_datum val)
{
	return gdbm_store((GDBM_FILE)dbm, DATUM(key), DATUM(val), GDBM_INSERT);
}

lac_datum lac_dbm_fetch(void* dbm, lac_datum key)
{
	datum val = gdbm_fetch((GDBM_FILE)dbm, DATUM(key));

	return (lac_datum){val.dptr, val.dsize};
}

int lac_dbm_delete(void* dbm, lac_datum key)
{
	return gdbm_delete((GDBM_FILE)dbm, DATUM(key));
}

int lac_dbm_dump(lac_dbm dbm, const char* file)
{
	return gdbm_dump((GDBM_FILE)dbm, file, GDBM_DUMP_FMT_ASCII, GDBM_WRCREAT, 0600);
}

int lac_dbm_load(lac_dbm* pdbm, const char* file)
{
	unsigned long e;

	return gdbm_load((GDBM_FILE*)pdbm, file, GDBM_DUMP_FMT_ASCII, 0, &e);
}
