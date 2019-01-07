#pragma once
#include "bit_move.h"
#include "utils.h"
#include "encoding.h"

using namespace convert;
using namespace utils;

namespace move {

	uint16_t row_right[0xffff];
	uint16_t row_left[0xffff];

	template<class F>
	void update(const uint16_t current_state, uint16_t* state_map, F move_row) {
		auto v = convert::to_vector(current_state);
		move_row(v);
		auto next_state = convert::to_bits(v);
		state_map[current_state] = next_state;
	}

	void init() {
		for (uint16_t current_state = 0; current_state < 0xffff; ++current_state) {
			update(current_state, row_left, move_row_left);
			update(current_state, row_right, move_row_right);
		}
	}

	template<uint16_t row_direction[0xffff]>
	static inline uint64_t board_move(uint64_t board)
	{
		uint16_t row0 = (board & ROW_MASK0) >> 48;
		uint16_t row1 = (board & ROW_MASK1) >> 32;
		uint16_t row2 = (board & ROW_MASK2) >> 16;
		uint16_t row3 = (board & ROW_MASK3);

		auto res0 = static_cast<uint64_t>(row_direction[row0]) << 48;
		auto res1 = static_cast<uint64_t>(row_direction[row1]) << 32;
		auto res2 = static_cast<uint64_t>(row_direction[row2]) << 16;
		auto res3 = static_cast<uint64_t>(row_direction[row3]);

		return res0 | res1 | res2 | res3;
	}

	uint64_t board_right(uint64_t board)
	{
		return board_move<row_right>(board);
	}

	uint64_t board_left(uint64_t board)
	{
		return board_move<row_left>(board);
	}

	uint64_t board_up(uint64_t board)
	{
		auto transposed = transpose(board);
		auto moved = board_left(transposed);
		return transpose(moved);
	}

	uint64_t board_down(uint64_t board)
	{
		auto transposed = transpose(board);
		auto moved = board_right(transposed);
		return transpose(moved);
	}
}
