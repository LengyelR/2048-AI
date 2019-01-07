#include "catch.hpp"
#include "game.h"
#include "utils.h"


TEST_CASE("get tile position", "[game]")
{
    REQUIRE(simulator::get_tile_position(0b0001'0001'0001'0001, 1) == 0);
    REQUIRE(simulator::get_tile_position(0b0001'0001'0001'0001, 3) == 8);
    REQUIRE(simulator::get_tile_position(0b0001'0001'0001'0001, 4) == 12);
    REQUIRE(simulator::get_tile_position(0b1000'0000'0000'0001, 2) == 15);
    REQUIRE(simulator::get_tile_position(0b0000'0010'0000'0001, 2) == 9);
    REQUIRE(simulator::get_tile_position(0b1111'1100'1111'1111, 6) == 5);
    REQUIRE(simulator::get_tile_position(0b1111'1100'1111'1111, 9) == 10);
    REQUIRE(simulator::get_tile_position(0b1111'1100'1111'1111, 11) == 12);
    REQUIRE(simulator::get_tile_position(0b0000'0010'0011'1001, 2)  == 3);
}

TEST_CASE("new board init", "[game]")
{
	for (int i=0; i<10; ++i)
	{
		auto board = simulator::init_board();
		auto cell_count = utils::count_zeros(board);
		REQUIRE(cell_count == 2);
	}
}

TEST_CASE("score", "[game]")
{
	REQUIRE(simulator::score(0x123456789abcdeff) == 98302);
	REQUIRE(simulator::score(0x000313003456fa03) == 33938);
	REQUIRE(simulator::score(0x0a0001000010001f) == 33798);
}

TEST_CASE("no more moves", "[game]")
{

	REQUIRE(simulator::any_possible_move(0x1111111111111111));
	REQUIRE(simulator::any_possible_move(0x0a0001000010001f));
	REQUIRE(simulator::any_possible_move(0x123111aaaaa33fa0));

	REQUIRE_FALSE(simulator::any_possible_move(0x2123456789abcdef));
	REQUIRE_FALSE(simulator::any_possible_move(0x1212212112122121));
	REQUIRE_FALSE(simulator::any_possible_move(0x124a213112c2212b));
}