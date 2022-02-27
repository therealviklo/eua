#pragma once
#include <cstdint>
#include <cstdlib>

void shithashpart(const unsigned char* data, size_t size, uint32_t* hash, size_t hashsize);
void shithashfinalise(uint32_t* hash, size_t hashsize);
void shithash(const unsigned char* data, size_t size, uint32_t* hash, size_t hashsize);