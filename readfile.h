#pragma once
#include <vector>
#include <cstring>
#include "utils.h"
#include "file.h"
#include "shithash.h"
#include "chacha20.h"

class FileReader
{
private:
	File f;
	size_t encsize;
	uint32_t nonce[2];
	uint32_t key[8];
	std::vector<unsigned char> buffer;
	size_t position;
	size_t globalposition;

	void load();
public:
	FileReader(const std::string& filename, const std::string& password);

	void read(unsigned char* buffer, size_t size);

	bool atend() const noexcept { return globalposition == encsize; }
};

void unpackarchive(const fs::path& arc, const std::string& password);