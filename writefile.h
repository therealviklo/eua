#pragma once
#include <vector>
#include "utils.h"
#include "file.h"
#include "shithash.h"
#include "chacha20.h"
#include "random.h"

class FileWriter
{
private:
	File f;
	uint32_t nonce[2];
	uint32_t key[8];
	uint32_t hash[16];
	std::vector<unsigned char> buffer;
	size_t position;
	size_t globalposition;

	void flush()
	{
		shithashpart(buffer.data(), position, hash, 16);
		chacha20(buffer.data(), position, key, (globalposition - position) / 64u + 1, nonce);
		f.write(buffer.data(), position);
		position = 0;
	}
public:
	FileWriter(const std::string& filename, const std::string& password);

	void write(const unsigned char* data, size_t size);

	void finalise();
};

void packfolder(const fs::path& arc, const std::string& password);