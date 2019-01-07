#include <algorithm> 
#include <random>
#include <iostream>
#include <set>
#include <intrin.h> 

#include "game.h"
#include "bit_move.h"

#define GET_CELL(board, idx) (board & cell_masks[idx]) >> (15 - idx) * 4

namespace simulator 
{
	//TODO: create constructor? init moves + init random generator...
	std::uniform_real_distribution<double> uniform_real(0.0, 1.0);
	std::uniform_int_distribution<int> uniform_int(0, 3);
	std::random_device rd;
	std::default_random_engine generator(rd());
	
    int get_random_move()
    {
        return uniform_int(generator);
    }

	uint64_t play_randomly(uint64_t board, int move)
	{
		auto copy = board;;
		board = simulator::do_move(board, move);
		if (board == copy) return board;

        //TODO: would it be faster to keep track of tried / untried moves? ...
        //     while (any_possible_move(board))
        for (int i=0; i<300; i++)
		{
			auto res = do_random_move(board);
			board = std::get<0>(res);
		}

		return board;
	}

	uint64_t place_random(uint64_t board)
	{
		uint16_t zero_cells = 0;
		for (int i = 0; i < 16; ++i)
		{
            if (board & cell_masks[i])
            {
                continue;
            }
            else
            {
                zero_cells |= 1 << i;
            }
		}
		
		if (zero_cells == 0)
		{
			return board;
		}

		std::uniform_int_distribution<int> uniform_discrete(1, __popcnt16(zero_cells));
		auto rand_idx = uniform_discrete(generator);

        auto idx = get_tile_position(zero_cells, rand_idx);
		auto new_cell = uniform_real(generator) > 0.9 ? 2ULL : 1ULL;

		return board | (new_cell << (60 - 4 * idx));
	}

    int get_tile_position(uint16_t zero_cells, int rand_idx)
    {
        int counter = 0;
        for (int i = 0; i < 16; ++i)
        {
            if (zero_cells & (1 << i))
            {
                counter++;
            }

            if (counter == rand_idx)
            {
                return i;
            }
        }
    }

	//TODO: would a gamescore method be needed?
	uint64_t score(uint64_t board)
	{
		uint64_t score = 0;
		for (int i = 0; i < 16; ++i)
		{
			auto cell = GET_CELL(board, i);
			score += ((2 << (cell-1)) & ~1ULL);
		}
		return score;
	}

	//TODO: merge with get_outcome...
	int get_biggest_tile(uint64_t board)
	{
		int biggest = 0;
		for (int i = 0; i < 16; ++i)
		{
			auto cell = GET_CELL(board, i);
			auto tile = 2 << (cell - 1);
			if (tile > biggest)
			{
				biggest = tile;
			}
		}
		return biggest;
	}

	bool get_outcome(uint64_t board, int end_tile)
	{
		for (int i = 0; i < 16; ++i)
		{
			auto cell = GET_CELL(board, i);
			auto tile = 2 << (cell - 1);
			if (tile >= end_tile) return true;
		}

		return false;
	}

	uint64_t init_board()
	{
		auto new_board = place_random(0);
		return place_random(new_board);
	}

	void init_moves()
	{
		move::init();
	}
	
	bool any_possible_move(uint64_t board)
	{
		auto is_up    = board != move::board_up(board);
		auto is_down  = board != move::board_down(board);
		auto is_left  = board != move::board_left(board);
		auto is_right = board != move::board_right(board);

		//TODO: can the compiler make the func return on first line, if is up true..
		return is_up || is_down || is_left || is_right;
	}

    // draw a random move
    std::tuple<uint64_t, int> do_random_move(uint64_t board)
	{
		auto rand_move = get_random_move();
		board = do_move(board, rand_move);

		return { board, rand_move };
	}

    // draw a move from possible_moves
	std::tuple<uint64_t, int> do_random_move(uint64_t board, 
		const std::set<int>& possible_moves)
	{
        auto rand_move = get_random_move();
		while (possible_moves.find(rand_move) == possible_moves.end())
		{
			rand_move = get_random_move();
		}
		board = do_move(board, rand_move);

		return { board, rand_move };
	}

	uint64_t do_move(uint64_t board, int move)
	{
        auto copy = board;

		switch (move)
		{
		case 0:
			board = move::board_up(board);
			break;
		case 1:
			board = move::board_down(board);
			break;
		case 2:
			board = move::board_left(board);
			break;
		case 3:
			board = move::board_right(board);
			break;
		default:
			throw std::exception("Unrecognised move!");
		}

		return board == copy ? board : place_random(board);
	}
}