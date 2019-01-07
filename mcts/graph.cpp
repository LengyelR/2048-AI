
#pragma once
#include <vector>
#include <set>
#include <algorithm>
#include <memory>
#include "game.h"
#include "graph.h"

namespace mcts
{
	Node::Node(uint64_t state, int move, Node* parent)
			:_visits(0.0), _wins(0.0), _move(move), _state(state),
			 _parent(parent), _possible_moves({0,1,2,3})
	{
	}

    Node::~Node()
    {
        //TODO: proper clean up needed...
    }

    Node* Node::add_child(uint64_t state, int move)
    {
        auto child = new Node(state, move, this);
        _children.emplace_back(child);
        _possible_moves.erase(move);
        return child;
    }

    bool Node::is_terminal()
    {
        return _children.size() == 0;
    }

    bool Node::is_fully_expanded()
    {
        return _possible_moves.size() == 0;
    }

    Node* Node::utc_select()
    {
        return *std::max_element(_children.begin(), _children.end(),
            [this](Node* a, Node* b)
            {
                return a->get_wins() / a->get_visits()
                    + sqrt(2 * log(this->get_visits()) / a->get_visits())
                    < b->get_wins() / b->get_visits()
                    + sqrt(2 * log(this->get_visits()) / b->get_visits());
            }
        );
    }

    int Node::get_move()
    {
        return _move;
    }

    void Node::update(float outcome)
    {
        _visits += 1;
        _wins += outcome;
    }

    uint64_t Node::get_state()
    {
        return _state;
    }

    Node* Node::get_parent()
    {
        return _parent;
    }

    float Node::get_visits()
    {
        return _visits;
    }

    float Node::get_wins()
    {
        return _wins;
    }

    std::vector<Node*> Node::get_children()
    {
        return _children;
    }

    std::set<int> Node::get_possible_moves()
    {
        return _possible_moves;
    }
}
