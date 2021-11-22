#include "shithash.h"

void shithashpart(const unsigned char* data, size_t size, uint32_t* hash, size_t hashsize)
{
	for (size_t i = 0; i < size; i++)
	{
		hash[(i / 4u) % hashsize] ^= data[i] << (8u * (i % 4u));
	}
}

void shithashfinalise(uint32_t* hash, size_t hashsize)
{
	for (size_t i = 0; i < hashsize; i++)
	{
		hash[i] = ~hash[i];
		hash[i] *= hash[i];
	}
}

void shithash(const unsigned char* data, size_t size, uint32_t* hash, size_t hashsize)
{
	for (size_t i = 0; i < hashsize; i++)
	{
		hash[i] = 0;
	}
	shithashpart(data, size, hash, hashsize);
	shithashfinalise(hash, hashsize);
}