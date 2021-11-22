#pragma once
#include <cstdio>
#include <cstdlib>

extern bool quiet;

inline void print(const char* str) noexcept
{
	if (!quiet)
	{
		std::printf("%s", str);
	}
}

inline void pressenter() noexcept
{
	if (!quiet)
	{
		std::printf("Tryck på enter för att fortsätta ... ");
		std::getchar();
	}
}

inline void printCrashMessage(const char* str) noexcept
{
	if (!quiet)
	{
		std::printf("Kritiskt körtidsfel: %s\n", str);
		pressenter();
	}
	std::exit(1);
}