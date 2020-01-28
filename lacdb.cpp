#include <map>
#include <string>
#include <string_view>
#include "lacdb.h"

using map = std::map<std::string,std::string>;

inline std::string make_string(lacdb_datum* datum)
{
	return std::string(datum->data, datum->data + datum->size);
}
inline std::string_view make_string_view(lacdb_datum* datum)
{
	return std::string_view(datum->data, datum->size);
}
inline lacdb_datum make_datum(const std::string& s)
{
	lacdb_datum d;

	d.data = const_cast<char*>(s.data());
	d.size = (int)s.size();

	return d;
}

#ifdef __cplusplus 
extern "C" {
#endif

lacdb lacdb_alloc() // returns std::map<K,V>*
{
	auto m = new map{};

	return m;
}
void lacdb_free(lacdb db)
{
	free((map*)db);
}

int lacdb_insert(lacdb db, lacdb_key key, lacdb_val val)
{
	map* mp = (map*)db;
	auto [i, b] = mp->insert(std::pair(make_string(key), make_string(val)));

	return b == true;
}

int lacdb_replace(lacdb db, lacdb_key key, lacdb_val val)
{
	map* mp = (map*)db;

	auto i = mp->find(make_string(key));
	if (i == mp->end())
		return false;

	i->second = make_string(val);

	return true;
}

lacdb_datum lacdb_find(lacdb db, lacdb_key key)
{
	lacdb_datum d{0,0};

	map* mp = (map*)db;
	const auto i = mp->find(make_string(key));

	return i == mp->end() ? d : make_datum(i->second);
}

int lacdb_erase(lacdb db, lacdb_key key)
{
	map* mp = (map*)db;
	/* string_view ??? */
	const auto i = mp->find(make_string(key));

	if (i == mp->end()) {
		return false;
	}

	mp->erase(i);

	return true;
}

#ifdef __cplusplus 
}
#endif
