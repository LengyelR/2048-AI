cimport binding
import torch
import numpy as np


# todo: rename...
def get_move_pure_mcts(vec_board, games_played=100):
    return binding.pure_get_move(vec_board, games_played)

def new_board():
    return binding.init_board()

def step(board, move):
    return binding.do_move(board, move)
    
def is_game_over(board):
    return not binding.any_possible_move(board)

def get_biggest_tile(board):
    return binding.get_biggest_tile(board)

def score(board):
    return binding.score(board)

def to_tensor(board):
    arr = np.array(binding.to_vectors(board), dtype=float)
    return torch.from_numpy(arr).float()

def get_scores(board):
    return binding.get_scores(board)

def cu_search_move(vec_board):
    return binding.search_move(vec_board)

def cu_init():
    return binding.init()


cu_init()  # gpu / cpu init on import
