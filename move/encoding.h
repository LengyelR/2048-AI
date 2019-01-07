#pragma once
#include <vector>

namespace convert {
	
	const uint64_t ROW_MASK0 = 0xffff000000000000;
	const uint64_t ROW_MASK1 = 0x0000ffff00000000;
	const uint64_t ROW_MASK2 = 0x00000000ffff0000;
	const uint64_t ROW_MASK3 = 0x000000000000ffff;

	uint16_t to_bits(const std::vector<int>& row);
	uint64_t to_bits(const std::vector<int>& row0, 
		const std::vector<int>& row1,
		const std::vector<int>& row2,
		const std::vector<int>& row3);

	std::vector<int> to_vector(const uint16_t row);
	std::vector<std::vector<int>> to_vectors(const uint64_t board);

}