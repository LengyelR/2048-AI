#pragma once
#include <cstdint>
#include <intrin.h> 
//TODO: make it platform independent?

namespace utils {
	static inline uint64_t transpose(uint64_t x)
	{
		uint64_t a1 = x & 0xf0f00f0ff0f00f0fULL;
		uint64_t a2 = x & 0x0000f0f00000f0f0ULL;
		uint64_t a3 = x & 0x0f0f00000f0f0000ULL;
		uint64_t a = a1 | (a2 << 12) | (a3 >> 12);
		uint64_t b1 = a & 0xff00ff0000ff00ffULL;
		uint64_t b2 = a & 0x00ff00ff00000000ULL;
		uint64_t b3 = a & 0x00000000ff00ff00ULL;
		return b1 | (b2 >> 24) | (b3 << 24);
	}

	static inline int count_zeros(uint64_t x)
	{
		//setting each non zero nibble's least significant bit
		x |= x >> 1;
		x |= x >> 2;

		// counting them
		x &= 0x1111111111111111UL;
		return __popcnt64(x);
	}
}
