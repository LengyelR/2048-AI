#include "bit_move.h"
#include "catch.hpp"

using row = uint16_t;
using vec_to_vec = std::vector<std::pair<std::vector<int>, std::vector<int>>>;
using row_to_row = std::vector<std::pair<row, row>>;
	

SCENARIO("moves", "[rows]") {
	GIVEN("zero or one item") {
		WHEN("moved to right") {
			vec_to_vec test_cases{
				{std::vector<int>{ 0,0,0,0 },  std::vector<int> { 0,0,0,0 }},
				{std::vector<int>{ 0,0,0,1 },  std::vector<int> { 0,0,0,1 }},
				{std::vector<int>{ 0,0,1,0 },  std::vector<int> { 0,0,0,1 }},
				{std::vector<int>{ 0,1,0,0 },  std::vector<int> { 0,0,0,1 }},
				{std::vector<int>{ 1,0,0,0 },  std::vector<int> { 0,0,0,1 }},
			};
			THEN("single right column") {
				for (auto pair : test_cases) {
					auto p0 = std::get<0>(pair);
					auto p1 = std::get<1>(pair);

					move::move_row_right(p0);
					REQUIRE(p0 == p1);
				}
			}
		}
		WHEN("move to left") {
			vec_to_vec test_cases{
				{std::vector<int>{ 0,0,0,0 },  std::vector<int> { 0,0,0,0 }},
				{std::vector<int>{ 0,0,0,1 },  std::vector<int> { 1,0,0,0 }},
				{std::vector<int>{ 0,0,1,0 },  std::vector<int> { 1,0,0,0 }},
				{std::vector<int>{ 0,1,0,0 },  std::vector<int> { 1,0,0,0 }},
				{std::vector<int>{ 1,0,0,0 },  std::vector<int> { 1,0,0,0 }},
			};
			THEN("single left column") {
				for (auto pair : test_cases) {
					auto p0 = std::get<0>(pair);
					auto p1 = std::get<1>(pair);

					move::move_row_left(p0);
					REQUIRE(p0 == p1);
				}
			}
		}
	}
}



SCENARIO("right moves (int)", "[rows]") {
	GIVEN("maps are initialised") {
		move::init();
		WHEN("row positions are simple") {
			row_to_row test_cases{
				{0x0044, 0x0005},
				{0x1010, 0x0002},
				{0x1111, 0x0022},
				{0x1212, 0x1212},
				{0x0222, 0x0023}
			};
			THEN("all board moves must be correct") {
				for (auto pair : test_cases) {
					auto p0 = std::get<0>(pair);
					auto p1 = std::get<1>(pair);

					auto res = move::row_right[p0];
					REQUIRE(res == p1);
				}
			}
		}
	}
}

