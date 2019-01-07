#include "encoding.h"

namespace convert {
	
	const uint16_t nibble0 = 0xf000;
	const uint16_t nibble1 = 0x0f00;
	const uint16_t nibble2 = 0x00f0;
	const uint16_t nibble3 = 0x000f;

	uint16_t to_bits(const std::vector<int>& row) 
    {
		uint16_t res = 0x0000;
		res |= (row[0] << 12) & nibble0;
		res |= (row[1] <<  8) & nibble1;
		res |= (row[2] <<  4) & nibble2;
		res |= (row[3]) & nibble3;

		return res;
	}

	uint64_t to_bits(const std::vector<int>& v0,
		const std::vector<int>& v1,
		const std::vector<int>& v2,
		const std::vector<int>& v3)
	{
		auto r0 = static_cast<uint64_t>(to_bits(v0)) << 48;
		auto r1 = static_cast<uint64_t>(to_bits(v1)) << 32;
		auto r2 = static_cast<uint64_t>(to_bits(v2)) << 16;
		auto r3 = static_cast<uint64_t>(to_bits(v3));

		return r0 | r1 | r2 | r3;
	}

	std::vector<int> to_vector(const uint16_t row) 
    {
		int cell0 = (row & nibble0) >> 12;
		int cell1 = (row & nibble1) >> 8;
		int cell2 = (row & nibble2) >> 4;
		int cell3 = (row & nibble3);
		return std::vector<int>{cell0, cell1, cell2, cell3};
	}

	std::vector<std::vector<int>> to_vectors(const uint64_t board)
	{
		uint16_t row0 = (board & ROW_MASK0) >> 48;
		uint16_t row1 = (board & ROW_MASK1) >> 32;
		uint16_t row2 = (board & ROW_MASK2) >> 16;
		uint16_t row3 = (board & ROW_MASK3);

		return std::vector<std::vector<int>>{
			to_vector(row0),
			to_vector(row1),
			to_vector(row2),
			to_vector(row3)
		};
	}
}