#pragma once
#include "vector_move.h"
#include "encoding.h"

namespace move {
	extern uint16_t row_right[0xffff];
	extern uint16_t row_left[0xffff];

	void init();
	uint64_t board_left(uint64_t board);
	uint64_t board_right(uint64_t board);
	uint64_t board_up(uint64_t board);
	uint64_t board_down(uint64_t board);

	//TODO: use enums...
	enum Direction { Up=0, Down=1, Left=2, Right=3};
}
