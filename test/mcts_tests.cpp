#include <iostream>
#include "catch.hpp"
#include "mcts.h"
#include "encoding.h"
#include "game.h"


TEST_CASE("pure mcts should win at least 60% of the games", "[prob]")
{
    //TODO: fixed random seed for the test...
	simulator::init_moves();
	int wins = 0;
	for (int i = 0; i < 100; ++i)
	{
        auto board = mcts::play_game<int(uint64_t, int)>(mcts::pure_get_move, 1000);
		if (simulator::get_outcome(board, 2048))
		{
			wins++;
		}
        std::cout << wins << " " << std::hex << board << std::dec << std::endl;
	}

	REQUIRE(wins >= 60);
}

TEST_CASE("final board should have no moves", "[mcts]")
{
    simulator::init_moves();
    std::vector<std::vector<int>> board1_v =
    {
        { 1, 2, 3, 4 },
        { 5, 6, 7, 8 },
        { 9, 2, 3, 4 },
        { 5, 6, 7, 8 } 
    };
    auto board1_b = convert::to_bits(board1_v[0], board1_v[1], board1_v[2], board1_v[3]);

    auto m = mcts::pure_get_move(board1_v, 10);
    auto any = simulator::any_possible_move(board1_b);
    REQUIRE(m == -1);
    REQUIRE_FALSE(any);
}


TEST_CASE("play game until end", "[mcts]")
{
    simulator::init_moves();
    for (int i = 0; i < 100; ++i)
    {
        auto board = mcts::play_game<int(uint64_t, int)>(mcts::pure_get_move, 5);
       
        auto m = mcts::pure_get_move(convert::to_vectors(board), 10);
        auto any = simulator::any_possible_move(board);
        REQUIRE(m == -1);
        REQUIRE_FALSE(any);
    }
}