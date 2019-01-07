#include "catch.hpp"
#include "bit_move.h"

SCENARIO("board move", "[board]")
{
	move::init();
	
	GIVEN("a simple board")
	{
		std::vector<int> r0{ 0, 0, 1, 1 };
		std::vector<int> r1{ 0, 2, 0, 0 };
		std::vector<int> r2{ 2, 2, 3, 3 };
		std::vector<int> r3{ 2, 2, 2, 2 };

		auto board_int = convert::to_bits(r0, r1, r2, r3);
		REQUIRE(board_int == 0x0011020022332222);

		WHEN("moved right")
		{
			auto moved = move::board_right(board_int);
			THEN("bit representation is correct") 
			{
				REQUIRE(moved == 0x0002000200340033);
			}

			auto board_after_moved = convert::to_vectors(moved);
			THEN("vector representation is correct")
			{
				REQUIRE(board_after_moved[0] == std::vector<int>{ 0, 0, 0, 2 });
				REQUIRE(board_after_moved[1] == std::vector<int>{ 0, 0, 0, 2 });
				REQUIRE(board_after_moved[2] == std::vector<int>{ 0, 0, 3, 4 });
				REQUIRE(board_after_moved[3] == std::vector<int>{ 0, 0, 3, 3 });
			}
		}

		WHEN("moved left")
		{
			auto moved_to_left = move::board_left(board_int);
			THEN("bit representation is correct")
			{
				REQUIRE(moved_to_left == 0x2000200034003300);
			}

			auto board_after_moved = convert::to_vectors(moved_to_left);
			THEN("vector representation is correct")
			{
				REQUIRE(board_after_moved[0] == std::vector<int>{ 2, 0, 0, 0 });
				REQUIRE(board_after_moved[1] == std::vector<int>{ 2, 0, 0, 0 });
				REQUIRE(board_after_moved[2] == std::vector<int>{ 3, 4, 0, 0 });
				REQUIRE(board_after_moved[3] == std::vector<int>{ 3, 3, 0, 0 });
			}
		}

		WHEN("moved up")
		{
			auto moved = move::board_up(board_int);
			THEN("bit representation is correct")
			{
				REQUIRE(moved == 0x3311023300220000);

			}

			auto board_after_move = convert::to_vectors(moved);
			THEN("vector representation is correct")
			{
				REQUIRE(board_after_move[0] == std::vector<int>{ 3, 3, 1, 1 });
				REQUIRE(board_after_move[1] == std::vector<int>{ 0, 2, 3, 3 });
				REQUIRE(board_after_move[2] == std::vector<int>{ 0, 0, 2, 2 });
				REQUIRE(board_after_move[3] == std::vector<int>{ 0, 0, 0, 0 });

			}
		}

		WHEN("moved down")
		{
			auto moved = move::board_down(board_int);
			THEN("bit representation is correct") 
			{
				REQUIRE(moved == 0x0000001102333322);
			}

			auto board_after_move = convert::to_vectors(moved);
			THEN("vector representation is correct")
			{
				REQUIRE(board_after_move[0] == std::vector<int>{ 0, 0, 0, 0 });
				REQUIRE(board_after_move[1] == std::vector<int>{ 0, 0, 1, 1 });
				REQUIRE(board_after_move[2] == std::vector<int>{ 0, 2, 3, 3 });
				REQUIRE(board_after_move[3] == std::vector<int>{ 3, 3, 2, 2 });
			}
		}
	}
}