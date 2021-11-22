#pragma once
#include <cstdio>
#include <stdexcept>
#include <filesystem>
#include "print.h"

namespace fs = std::filesystem;

template <typename T>
void byteswap(T& t)
{
	T tmp = 0;
	for (int i = 0; i < sizeof(T); i++)
	{
		tmp <<= 8;
		tmp |= t & 0xff;
		t >>= 8;
	}
	t = tmp;
}

template <typename T, size_t size>
void byteswaparray(T (&t)[size])
{
	for (size_t i = 0; i < size; i++)
	{
		byteswap(t[i]);
	}
}

inline std::wstring string2wstring(const char* s)
{
	std::mbstate_t state = std::mbstate_t();
	const size_t size = std::mbsrtowcs(
		nullptr,
		&s,
		0,
		&state
	) + 1;
	if (size - 1 == static_cast<size_t>(-1))
		throw std::runtime_error("Failed to convert UTF-8 string to UTF-16 string");
	std::wstring ws(size, L'\0');
	if (std::mbsrtowcs(
			&ws[0],
			&s,
			size,
			&state
		) == static_cast<size_t>(-1))
		throw std::runtime_error("Failed to convert UTF-8 string to UTF-16 string");
	return ws;
}

inline std::wstring string2wstring(const std::string& s)
{
	return string2wstring(s.c_str());
}