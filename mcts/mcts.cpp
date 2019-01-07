#pragma once
#include "mcts.h"

namespace mcts
{

	int pure_get_move(const std::vector<std::vector<int>>& vec_board, int games_played)
	{
        auto board = convert::to_bits(vec_board[0], vec_board[1], vec_board[2], vec_board[3]);
        return pure_get_move(board, games_played);
	}

    int pure_get_move(uint64_t board, int games_played)
    {
        std::set<int> possible_moves{ 0,1,2,3 };

        while (possible_moves.size())
        {
            auto copy = board;
            auto m = mcts::algorithm::pure_search(board, games_played, possible_moves);
            board = simulator::do_move(board, m);

            if (copy == board)
            {
                possible_moves.erase(m);
            }
            else
            {
                return m;
            }
        }

        return -1;
    }
}