#include "checkpass.h"

bool checkIfPasswordIsValidForFile(const std::string& filename, const std::string& password)
{
	uint32_t key[8];
	shithash((const unsigned char*)password.data(), password.length(), key, 8);

	File f(filename.c_str(), "rb");
	const uintmax_t fsize = fs::file_size(filename);

	uint32_t nonce[2];
	f.readwhole(nonce, sizeof(nonce));
	if constexpr (std::endian::native == std::endian::big)
	{
		byteswaparray(nonce);
	}
	uint32_t correcthash[16];
	f.readwhole(correcthash, sizeof(correcthash));
	chacha20((unsigned char*)correcthash, sizeof(correcthash), key, 0, nonce);
	if constexpr (std::endian::native == std::endian::big)
	{
		byteswaparray(correcthash);
	}

	uint32_t calculatedhash[16];
	for (int i = 0; i < 16; i++)
	{
		calculatedhash[i] = 0;
	}

	size_t pos = sizeof(nonce) + sizeof(correcthash);
	std::vector<unsigned char> buffer(std::min<size_t>(fsize - pos, filebufferchunksize));
	while (pos < fsize)
	{
		const size_t readsize = f.read(buffer.data(), buffer.size());
		chacha20(buffer.data(), readsize, key, (pos - sizeof(nonce)) / 64u, nonce);
		shithashpart(buffer.data(), readsize, calculatedhash, 16);
		pos += filebufferchunksize;
	}
	shithashfinalise(calculatedhash, 16);

	for (int i = 0; i < 16; i++)
	{
		if (correcthash[i] != calculatedhash[i])
			return false;
	}
	return true;
}