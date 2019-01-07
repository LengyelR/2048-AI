#pragma once
#include <vector>
#include <set>
#include <algorithm>
#include <memory>
#include "game.h"

namespace mcts 
{
	class Node
	{
	public:
		Node(uint64_t state, int move, Node* parent);
		~Node();

		Node* add_child(uint64_t state, int move);

		bool is_terminal();

		bool is_fully_expanded();

		Node* utc_select();

		int get_move();

		void update(float outcome);

		uint64_t get_state();

		Node* get_parent();

		float get_visits();

		float get_wins();

		std::vector<Node*> get_children();

		std::set<int> get_possible_moves();

	private:
		float _visits;
		float _wins;
		int _move;
		uint64_t _state;
		Node* _parent;
		std::vector<Node*> _children;
		std::set<int> _possible_moves;
	};
}
