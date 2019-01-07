import simulator
from controller import ChromeDebuggerControl
from controller import GameControl


def play(game, limit, debug=True):
    status = game.get_status()

    counter = 0
    while status != 'ended':
        board = game.get_board()
        # m = simulator.get_move_pure_mcts(board, limit)
        m = simulator.cu_search_move(board)

        if debug:
            print(counter, m, board)

        game.execute_move(m)

        status = game.get_status()
        if status == 'won':
            game.continue_game()
        counter += 1

    return game.get_max_tile()


if __name__ == "__main__":
    import sys
    import time
    
    iter_count = 10000
    debug = True

    if len(sys.argv) > 1:
        iter_count = int(sys.argv[1])
    if len(sys.argv) > 2:
        debug = int(sys.argv[2])

    ctrl = ChromeDebuggerControl(9222)
    gg = GameControl(ctrl)
    stats = []
    
    wins = 0
    losses = 0
    four_k = 0
    maximum = 0

    for idx in range(100):
        start = time.time()
        res = play(gg, iter_count, debug)
        time_spent = time.time()-start

        biggest_tile = 2**res

        if biggest_tile == 2048:
            four_k += 1

        if biggest_tile >= 2048:
            wins += 1
        else:
            losses += 1

        if biggest_tile > maximum:
            maximum = biggest_tile

        stats.append(res)
        print(idx, biggest_tile, gg.get_score(), time_spent, 'ms')
        gg.restart_game()

    print('wins:', wins)
    print('4096:', four_k)
    print('losses:', losses)
    print('max:', maximum)
    print(stats)
