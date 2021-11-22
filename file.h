#pragma once
#include <memory>
#include <cstdio>
#include <stdexcept>

inline constexpr size_t filebufferchunksize = 0x0fffff00; // Måste vara multipel av 64

using namespace std::literals;

using UFile = std::unique_ptr<FILE, decltype([](FILE* f){ std::fclose(f); })>;
class File : public UFile
{
public:
	File(const char* filename, const char* mode) :
		UFile(std::fopen(filename, mode))
	{
		if (!*this)
			throw std::runtime_error("Failed to open file "s + filename);
	}

	size_t read(void* buffer, size_t size)
	{
		if (!size) return 0;
		return std::fread(buffer, 1, size, get());
	}

	void readwhole(void* buffer, size_t size)
	{
		if (size)
		{
			if (!std::fread(buffer, size, 1, get()))
				throw std::runtime_error("Failed to read from file");
		}
	}

	void write(const void* buffer, size_t size)
	{
		if (size)
		{
			if (!std::fwrite(buffer, size, 1, get()))
				throw std::runtime_error("Failed to write to file");
		}
	}

	void seekl(long pos)
	{
		if (std::fseek(get(), pos, SEEK_SET))
			throw std::runtime_error("Failed to seek in file");
	}

	void skip(long num)
	{
		if (std::fseek(get(), num, SEEK_CUR))
			throw std::runtime_error("Failed to seek in file");
	}
};