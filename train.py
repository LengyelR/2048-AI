import numpy as np
import torch
import torch.optim as optim
import torch.nn.functional as F
from torch.utils.data import DataLoader
from torch.autograd import Variable

import simulator as sim
import model
from data import GameHistory


def pretty(board):
    s = hex(board)[2:]
    s = s.zfill(16)

    for idx, tile in enumerate(s):
        tile = int(tile, 16)
        print(0 if tile == 0 else 2**tile, end='\t')
        if (idx+1) % 4 == 0:
            print('')
    print('')


def normalise_board(board):
    tensor = sim.to_tensor(board)
    normed = (tensor - torch.mean(tensor)) / torch.std(tensor)
    return normed.view(1, 1, 4, 4)


def evaluate(policy):
    def play():
        counter = 0
        board = sim.new_board()

        while True:
            counter += 1
            copy = board

            p, v = policy(normalise_board(board))
            best_moves = np.argsort(-p.data, axis=1).view(4)

            for m in best_moves:
                board = sim.step(board, m)
                if board == copy:
                    continue

            if sim.is_game_over(board):
                break

            if counter % 100 == 0:
                print('PLAYING...', counter, 'policy moves:', p, 'value:', v)

        return counter, sim.get_biggest_tile(board)

    print('-' * 50)
    for _ in range(5):
        print(play())
    print('-' * 50)


def mcts_play():
    board = sim.new_board()

    states = []
    action_pis = []

    def mcts_step():
        scores = sim.get_scores(board)
        res = np.asarray(scores)
        return (res - np.mean(res)) / np.std(res)

    counter = 0
    while True:
        counter += 1

        pi = mcts_step()
        action_pis.append(pi)
        state = normalise_board(board)
        states.append(state)

        board = sim.step(board, np.argmax(pi))

        if sim.is_game_over(board):
            break

        if counter % 100 == 0:
            print(counter, 'mcts moves:', pi)

    outcomes = np.ones(len(action_pis))
    outcome = sim.get_biggest_tile(board)
    if outcome < 2048:
        outcomes *= -1

    print('biggest tile:', outcome)
    return states, action_pis, outcomes


def update(opt, training_data, policy):
    train_loader = DataLoader(training_data, batch_size=16, shuffle=True)

    for epoch in range(1):
        for i, data in enumerate(train_loader):

            loss_list = []
            for state, pi, z in zip(*data):

                action_p, v = policy(state)
                p = action_p.view(4).float()
                v = v.float()
                z = z.float()

                cross_entropy = -F.softmax(pi.float(), dim=0) * torch.log(F.softmax(p, dim=0))
                mean_squared = (z-v)**2
                loss_list.append(cross_entropy.sum() + mean_squared)  # regularisation included in optimiser

            opt.zero_grad()

            loss_tensor = torch.cat(loss_list)
            loss = loss_tensor.sum()
            if i % 25 == 0:
                print(i, 'loss', loss.item())
            loss.backward()
            opt.step()


def main():
    from pathlib import Path
    from datetime import datetime as dt

    game_count = 5

    policy = model.Net(model.ResidualBlock, model.ValueHead, model.PolicyHead)

    original_policy = model.Net(model.ResidualBlock, model.ValueHead, model.PolicyHead)
    original_policy.load_state_dict(policy.state_dict())

    opt = optim.SGD(policy.parameters(), lr=1e-4, momentum=0.9, weight_decay=1e-5)

    model_path = Path('10_policy.pt')
    if model_path.exists():
        policy = torch.load(model_path)
        evaluate(policy)

    for iteration in range(100):

        training_data = GameHistory()
        for game_idx in range(game_count):
            print(dt.now(), 'batch:', iteration, 'game:', game_idx)
            sar = mcts_play()
            training_data.extend(*sar)

        update(opt, training_data, policy)

        if iteration % 10 == 0:
            torch.save(policy, f'{iteration}_policy.pt')
            evaluate(policy)


if __name__ == "__main__":
    main()
