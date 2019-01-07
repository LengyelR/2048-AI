#include <iostream>
#include <functional>
#include <algorithm>
#include <chrono>

#include "game.h"
#include "mcts.h"

using ms = std::chrono::milliseconds;
using namespace std::chrono;

double generate_end_move_stats()
{
    double avg = 0;
    int iter = 8000;
    for (int i = 0; i < iter; ++i)
    {
        int counter = 0;
        auto board = simulator::init_board();
        while (simulator::any_possible_move(board))
        {
            auto res = simulator::do_random_move(board);
            board = std::get<0>(res);
            ++counter;
        }
        std::cout << counter << std::endl;
        avg += counter;
    }

    return avg / iter;
}

void get_game_stats()
{
    simulator::init_moves();
    int wins = 0;
    int losses = 0;
    int four_k = 0;
    int maximum = 0;
    for (int i = 0; i < 100; i++)
    {
        auto start = steady_clock::now();
        auto terminal_state = mcts::play_game<int(uint64_t, int)>(mcts::pure_get_move, 100);
        auto duration = duration_cast<ms>(steady_clock::now() - start);
        int biggest_tile = simulator::get_biggest_tile(terminal_state);

        if (biggest_tile >= 2048) wins++;
        else losses++;

        if (biggest_tile == 4096) four_k++;


        if (biggest_tile > maximum) maximum = biggest_tile;

        std::cout << i << " "
            << biggest_tile << " "
            << std::hex << terminal_state << " "
            << std::dec << duration.count() << "ms"
            << std::endl;
    }

    std::cout << "wins: " << wins << std::endl;
    std::cout << "4096: " << four_k << std::endl;
    std::cout << "losses: " << losses << std::endl;
    std::cout << "max: " << maximum << std::endl;
}

void compare_cuda_like()
{
    const int stop = 300;
    const int grid_size = 4096;
    const int block_size = 512;
    const int N = block_size * grid_size;

    auto boards = new uint64_t[N];

    simulator::init_moves();
    auto start = steady_clock::now();

    auto board = simulator::init_board();
    for (int i = 0; i < 100; ++i)
    {
        auto best_move = mcts::pure_get_move(board, N);
        board = simulator::do_move(board, best_move);
    }

    auto duration = duration_cast<ms>(steady_clock::now() - start);

    std::cout << std::dec << duration.count() << " ms\n";
    std::cout << std::hex << board << "\n";
    std::cout << std::dec << simulator::score(board) << "\n";

    delete[] boards;
}

int main()
{
    compare_cuda_like();
}