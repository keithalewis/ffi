// ffi_parse.h - parse text input
#pragma once
#include <cassert>
#include <cctype>
#include <utility>
#include <vector>

namespace ffi {

	using token_view = std::pair<const char*, const char*>;

	inline bool empty(const token_view& v)
	{
		return v.first == v.second;
	}

	// v.first points at bad parse
	inline bool error(const token_view& v)
	{
		return v.second == nullptr;
	}

	// end of token boundary
	inline bool boundary(const char* b, const char* e)
	{
		return b == e || isspace(*b);
	}

	// skip space or nonspace
	inline const char* skip_space(const char* b, const char* e, bool space = true)
	{
		if (b >= e)
			return nullptr;

		while (b < e and (space ? isspace(*b) : !isspace(*b))) {
			++b;
		}

		assert (b <= e);

		return b;
	}
	inline const char* skip_space(const token_view& v, bool space = true)
	{
		return skip_space(v.first, v.second, space);
	}

	// match delimiter at same nesting level
	// return pointer past matching right delimiter
	inline const char* next_match(const char* b, const char* e, char l = '{', char r = '}')
	{
		if (b >= e)
			return nullptr;

		if (*b != l)
			return nullptr;

		std::size_t level = 1;
		while (++b < e && level != 0) {
			if (*b == '\\') {
				++b;
				if (b == e)
					return nullptr;
			}
			else {
				if (*b == r)
					--level;
				else if (*b == l)
					++level;
			}
		}

		return level == 0 ? b : nullptr;
	}
	inline const char* next_match(const token_view& v, char l = '{', char r = '}')
	{
		return next_match(v.first, v.second, l, r);
	}

	inline const char* next_quote(const char* b, const char* e, const char q = '"')
	{
		return next_match(b, e, q, q);
	}
	inline const char* next_quote(const token_view& v, const char q = '"')
	{
		return next_quote(v.first, v.second, q);
	}

	inline token_view parse_token(const char* b, const char* e)
	{
		if (b == nullptr)
			return token_view(e, nullptr);

		if (b == e)
			return token_view(b, e);

		if (b > e)
			return token_view(b, nullptr); // error

		const char* e_;

		b = skip_space(b, e);

		if (b == e)
			return token_view(e,e); // empty

		if (*b == '"') {
			e_ = next_quote(b, e);
		}
		else if (*b == '{') {
			e_ = next_match(b, e);
		}
		else {
			e_ = skip_space(b, e, false);
		}

		return token_view(b, e_);
	}

	// vector of token_views, including errors
	inline std::vector<token_view> parse_line(const char* b, const char* e)
	{
		std::vector<token_view> v;

		token_view t = parse_token(b, e);
		while (!empty(t)) {
			v.push_back(t);

			if (!boundary(t.second, e)) {
				v.push_back(token_view(t.second, nullptr));

				break;
			}

			t = parse_token(t.second, e);
		}

		return v;
	}

}
