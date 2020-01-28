/* lacdb.h - key-value database */
#pragma once
#include <string.h>

#ifdef __cplusplus 
extern "C" {
#endif

typedef struct {
	char* data;
	int   size;
} lacdb_datum;
typedef lacdb_datum* lacdb_key;
typedef lacdb_datum* lacdb_val;

inline lacdb_datum lacdb_make_datumn(char* s, int n)
{
	lacdb_datum d;

	d.data = s;
	d.size = n;

	return d;
}
inline lacdb_datum lacdb_make_datum(char* s)
{
	return lacdb_make_datumn(s, (int)strlen(s));
}

typedef void* lacdb;
lacdb lacdb_alloc(); // returns std::map<K,V>*
void lacdb_free(lacdb);
int lacdb_insert(lacdb, lacdb_key, lacdb_val);
int lacdb_replace(lacdb, lacdb_key, lacdb_val);
lacdb_datum lacdb_find(lacdb, lacdb_key);
int lacdb_erase(lacdb, lacdb_key);

#ifdef __cplusplus 
}
#endif
