// ffi_parse.h - parse text input
#pragma once
#include <cassert>
#include <cctype>
#include <utility>
#include <vector>

namespace ffi {

	using token_view = std::pair<const char*, const char*>;

	inline const char* skip_space(const char* b, const char* e, bool space = true)
	{
		while (b < e and (space ? isspace(*b) : !isspace(*b))) {
			++b;
		}

		return b;
	}
	inline const char* skip_space(const token_view& v, bool space = true)
	{
		return skip_space(v.first, v.second, space);
	}

	inline const char* next_quote(const char* b, const char* e, const char q = '"')
	{
		while (b < e && *b != q) {
			if (*b == '\\') {
				++b;
				if (b < e)
					++b;
			}
			else {
				++b;
			}
		}

		return b == e ? nullptr : b; // b == e if no next quote
	}
	inline const char* next_quote(const token_view& v, const char q = '"')
	{
		return next_quote(v.first, v.second, q);
	}

	// match delimiter at same nesting level
	// return pointer past matching right delimiter
	inline const char* next_match(const char* b, const char* e, char l = '{', char r = '}')
	{
		if (*b != l)
			return nullptr;

		++b;
		std::size_t level = 1;
		while (level != 0  && b < e) {
			if (*b == '\\') {
				++b;
				if (b < e)
					++b;
			}
			else {
				if (*b == l)
					++level;
				else if (*b == r)
					--level;
				++b;
			}
		}

		if (level != 0)
			return nullptr;

		return b;
	}
	inline const char* next_match(const token_view& v, char l = '{', char r = '}')
	{
		return next_match(v.first, v.second, l, r);
	}

	inline token_view parse_token(const char* b, const char* e)
	{
		const char* e_;

		b = skip_space(b, e);

		if (b == e) {
			return token_view(e,e);
		}

		if (*b == '"') {
			++b;
			e_ = next_quote(b, e);

			return token_view(e_ == nullptr ? e_ : b, e_);
		}

		if (*b == '{') {
			e_ = next_match(b, e);

			return token_view(e_ == nullptr ? e_ : b, e_);
		}

		e_ = skip_space(b, e, false);

		return token_view(b, e_);
	}

	inline std::vector<token_view> parse_line(const char* b, const char* e)
	{
		std::vector<token_view> v;

		return v;
	}

}
