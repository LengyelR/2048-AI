#include "encoding.h"
#include "catch.hpp"

TEST_CASE("encode rows", "[rows]") {
	std::vector<std::pair<std::vector<int>, uint16_t>> test_cases{
		{std::vector<int>{ 0,  0,  0,  0}, 0x0000},
		{std::vector<int>{12,  0, 12,  0}, 0xc0c0},
		{std::vector<int>{ 9, 10, 11, 12}, 0x9abc},
		{std::vector<int>{ 0,  1,  2,  3}, 0x0123},
		{std::vector<int>{10,  9,  8,  7}, 0xa987},
		{std::vector<int>{15, 15, 15, 15}, 0xffff},
		{std::vector<int>{ 0,  4,  4,  4}, 0x0444}
	};

	for (auto pair : test_cases) {
		auto v = std::get<0>(pair);
		auto v_res = std::get<1>(pair);

		auto res = convert::to_bits(v);
		REQUIRE(res == v_res);
	}
}

TEST_CASE("decode rows", "[rows]") {
	std::vector<std::pair<uint16_t, std::vector<int>>> test_cases{
		{ 0x0000, std::vector<int>{ 0,  0,  0,  0}},
		{ 0xc0c0, std::vector<int>{12,  0, 12,  0}},
		{ 0x9abc, std::vector<int>{ 9, 10, 11, 12}},
		{ 0x0123, std::vector<int>{ 0,  1,  2,  3}},
		{ 0xa987, std::vector<int>{10,  9,  8,  7}},
		{ 0xffff, std::vector<int>{15, 15, 15, 15}},
		{ 0x0444, std::vector<int>{ 0,  4,  4,  4}}
	};

	for (auto pair : test_cases) {
		auto b = std::get<0>(pair);
		auto v_res = std::get<1>(pair);

		auto v = convert::to_vector(b);
		REQUIRE(v == v_res);
	}
}

TEST_CASE("encode board", "[board]"){
	std::vector<int> r0{ 0, 0, 1, 1 };
	std::vector<int> r1{ 0, 2, 0, 0 };
	std::vector<int> r2{ 2, 2, 3, 3 };
	std::vector<int> r3{ 2, 2, 2, 2 };
	uint64_t res = 0x0011020022332222;

	auto vectors = convert::to_vectors(res);
	REQUIRE(vectors[0] == r0);
	REQUIRE(vectors[1] == r1);
	REQUIRE(vectors[2] == r2);
	REQUIRE(vectors[3] == r3);
}