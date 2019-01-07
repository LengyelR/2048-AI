from libcpp.vector cimport vector
from libcpp cimport bool

ctypedef unsigned long long uint64_t

cdef extern from "mcts.h" namespace "mcts":
    int pure_get_move(vector[vector[int]] vec_board, int games_played)

cdef extern from "algorithm.h" namespace "mcts::algorithm":
    vector[double] get_scores(uint64_t board)

cdef extern from "game.h" namespace "simulator":
    void init_moves()
    uint64_t init_board()
    uint64_t do_move(uint64_t board, int move)
    int get_biggest_tile(uint64_t board)
    bool any_possible_move(uint64_t board)
    uint64_t score(uint64_t board)

cdef extern from "encoding.h" namespace "convert":
    vector[vector[int]] to_vectors(uint64_t board)

cdef extern from "cuda_move.cuh" namespace "cuda_sim":
    int search_move(vector[vector[int]] vec_board)
    void init()
