#pragma once
#include "graph.h"
#include <iostream>

namespace mcts::algorithm
{
    double get_score_of_move(uint64_t board, int iter_limit, int move)
    {
        double score = 0;

        for (int i = 0; i < iter_limit; ++i)
        {
            auto res = simulator::play_randomly(board, move);
            score += simulator::score(res);
        }

        return score / iter_limit;
    }

	int get_best_move(const std::vector<mcts::Node*>& children)
	{
		auto most_visited = **std::max_element(children.begin(), children.end(),
			[](Node* a, Node* b) {return a->get_visits() < b->get_visits(); });
		auto best_move = most_visited.get_move();

		return best_move;
	}

	int search(uint64_t board, int iter_limit)
	{
		auto root = Node(board, 0, nullptr);
		auto node = &root;

		for (int i = 0; i < iter_limit; ++i)
		{
			board = root.get_state();

			//TODO: could be replaced by a parallel for (the 4 actions)
			//TODO: in this case, terminal and fully expanded the same...
			while (!node->is_terminal() && node->is_fully_expanded())
			{
				node = node->utc_select(); // descend the node
				board = simulator::do_move(board, node->get_move());
			}

			if (!node->is_fully_expanded())
			{
				auto res = simulator::do_random_move(board, node->get_possible_moves());
				board = std::get<0>(res);
				auto rnd_move = std::get<1>(res);

				node = node->add_child(board, rnd_move); // descend...
			}

			while (simulator::any_possible_move(board))
			{
				//TODO: rollout policy
			}

			while (node->get_parent())
			{
				auto outcome = simulator::get_outcome(board, 2048);
				node->update(outcome);
				node = node->get_parent();
			}
		}

		return get_best_move(root.get_children());
	}

	
    int pure_search(uint64_t board, int iter_limit, const std::set<int>& possible_moves)
	{
        std::vector<double> scores;

        auto append_score = [&possible_moves, &scores, board, iter_limit](auto move)
        {
            if (possible_moves.find(move) != possible_moves.end())
                scores.emplace_back(get_score_of_move(board, iter_limit, move));
            else
                scores.emplace_back(-1);
        };

        append_score(0);
        append_score(1);
        append_score(2);
        append_score(3);


        auto max = std::max_element(scores.begin(), scores.end());
        return std::distance(scores.begin(), max);
	}

    std::vector<double> get_scores(uint64_t board)
    {
        std::vector<double> scores;
        scores.emplace_back(get_score_of_move(board, 100, 0));
        scores.emplace_back(get_score_of_move(board, 100, 1));
        scores.emplace_back(get_score_of_move(board, 100, 2));
        scores.emplace_back(get_score_of_move(board, 100, 3));
        return scores;
    }
}