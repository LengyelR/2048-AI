#pragma once
#include "graph.h"

namespace mcts::algorithm
{
	int search(uint64_t board, int iter_limit);
	int pure_search(uint64_t board, int iter_limit, const std::set<int>& possible_moves);
    std::vector<double> get_scores(uint64_t board);
}