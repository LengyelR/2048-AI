#pragma once
#include "algorithm.h"
#include "encoding.h"

namespace mcts
{
    template<class F>
    inline uint64_t play_game(F find_best_move, int games_played)
    {
        auto board = simulator::init_board();

        while (true)
        {
            auto m = find_best_move(board, games_played);
            
            if (m < 0) //no possible move
                return board;

            board = simulator::do_move(board, m);
        }

        return board;
    }

    int pure_get_move(uint64_t board, int games_played);
	int pure_get_move(const std::vector<std::vector<int>>& vec_board, int games_played);
}