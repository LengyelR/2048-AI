#pragma once
#include <cstdint>
#include <set>

//TODO: rename file?
namespace simulator {
	const uint64_t cell_masks[16]
	{
		0xf000000000000000,
		0x0f00000000000000,
		0x00f0000000000000,
		0x000f000000000000,
		0x0000f00000000000,
		0x00000f0000000000,
		0x000000f000000000,
		0x0000000f00000000,
		0x00000000f0000000,
		0x000000000f000000,
		0x0000000000f00000,
		0x00000000000f0000,
		0x000000000000f000,
		0x0000000000000f00,
		0x00000000000000f0,
		0x000000000000000f
	};

	uint64_t init_board();
	void init_moves();
	
	uint64_t score(uint64_t board);
	bool get_outcome(uint64_t board, int end_tile);
	int get_biggest_tile(uint64_t board);

	uint64_t place_random(uint64_t board);
    int get_tile_position(uint16_t zero_cells, int rand_idx);

	uint64_t do_move(uint64_t board, int move);
	std::tuple<uint64_t, int> do_random_move(uint64_t board);
	std::tuple<uint64_t, int> do_random_move(uint64_t board, const std::set<int>& possible_moves);
	
	bool any_possible_move(uint64_t board);

	uint64_t play_randomly(uint64_t board, int move);
}