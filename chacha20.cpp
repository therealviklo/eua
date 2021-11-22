#include "chacha20.h"

void chacha20(unsigned char* data, size_t size, uint32_t key[8], uint64_t counter, uint32_t nonce[2])
{
	size_t currPos = 0;
	while (currPos < size)
	{
		// Initiera state
		uint32_t state[16];
		state[0] = 0x61707865;
		state[1] = 0x3320646e;
		state[2] = 0x79622d32;
		state[3] = 0x6b206574;
		for (int i = 0; i < 8; i++)
		{
			state[4 + i] = key[i];
		}
		state[12] = static_cast<uint32_t>(counter);
		state[13] = static_cast<uint32_t>(counter >> 32u);
		for (int i = 0; i < 2; i++)
		{
			state[14 + i] = nonce[i];
		}

		// Kör rundor
		auto qr = [](uint32_t& a, uint32_t& b, uint32_t& c, uint32_t& d) {
			a += b;
			d ^= a;
			d = std::rotl<uint32_t>(d, 16);
			c += d;
			b ^= c;
			b = std::rotl<uint32_t>(d, 12);
			a += b;
			d ^= a;
			d = std::rotl<uint32_t>(d, 8);
			c += d;
			b ^= c;
			b = std::rotl<uint32_t>(d, 7);
		};
		for (int i = 0; i < 10; i++)
		{
			qr(state[0], state[4], state[8], state[12]);
			qr(state[1], state[5], state[9], state[13]);
			qr(state[2], state[6], state[10], state[14]);
			qr(state[3], state[7], state[11], state[15]);
			
			qr(state[0], state[5], state[10], state[15]);
			qr(state[1], state[6], state[11], state[12]);
			qr(state[2], state[7], state[8], state[13]);
			qr(state[3], state[4], state[9], state[14]);
		}

		// Lägg till state som det var från början
		state[0] += 0x61707865;
		state[1] += 0x3320646e;
		state[2] += 0x79622d32;
		state[3] += 0x6b206574;
		for (int i = 0; i < 8; i++)
		{
			state[4 + i] += key[i];
		}
		state[12] += static_cast<uint32_t>(counter);
		state[13] += static_cast<uint32_t>(counter >> 32u);
		for (int i = 0; i < 2; i++)
		{
			state[14 + i] += nonce[i];
		}

		// Xor:a
		for (int i = 0; i < 64 && currPos + i < size; i++)
		{
			data[currPos + i] ^= state[i / 4u] >> (8u * (i % 4u));
		}
		currPos += 64;

		counter++;
	}
}