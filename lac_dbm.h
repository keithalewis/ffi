// lac_dbm.h - key value map abstraction
#pragma once

// key and value type
typedef struct
{
  char *data;
  int   size;
} lac_datum;

// opaque handle
typedef void* lac_dbm;

// handle to key-value store
lac_dbm lac_dbm_open (const char* name);
int   lac_dbm_close(lac_dbm dbm);

// return 0 on success
int   lac_dbm_replace(lac_dbm dbm, lac_datum key, lac_datum val);
int   lac_dbm_insert (lac_dbm dbm, lac_datum key, lac_datum val);

// data = 0 on failure
lac_datum lac_dbm_fetch (lac_dbm dbm, lac_datum key);
int       lac_dbm_delete(lac_dbm dbm, lac_datum key);

int lac_dbm_dump(lac_dbm dbm, const char* file);
int lac_dbm_load(lac_dbm* pdbm, const char* file);
