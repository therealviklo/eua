#pragma once
#include <cstdint>
#include <bit>

void chacha20(unsigned char* data, size_t size, uint32_t key[8], uint64_t counter, uint32_t nonce[2]);