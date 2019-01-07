#include <iostream>
#include <chrono>
#include "game.h"
#include "cuda_move.cuh"


using ms = std::chrono::milliseconds;
using namespace std::chrono;

uint64_t cu_play()
{
    int i = 0;
    auto board = simulator::init_board();
    while (simulator::any_possible_move(board))
    {
        auto best_move = cuda_sim::search_move(board);
        board = simulator::do_move(board, best_move);
        if (i % 10 == 0) std::cout << std::dec << i << ": "  << std::hex << board << "\n";
        ++i;
    }

    return board;
}

void cu_get_game_stats()
{
    cuda_sim::init();
    int wins = 0;
    int losses = 0;
    int four_k = 0;
    int maximum = 0;
    for (int i = 0; i < 100; i++)
    {
        auto start = steady_clock::now();
        auto terminal_state = cu_play();
        auto duration = duration_cast<ms>(steady_clock::now() - start);
        int biggest_tile = simulator::get_biggest_tile(terminal_state);

        if (biggest_tile >= 2048) wins++;
        else losses++;

        if (biggest_tile == 4096) four_k++;


        if (biggest_tile > maximum) maximum = biggest_tile;

        std::cout << std::dec << i << " "
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


int main()
{
    cuda_sim::init();
    for (int i = 0; i < 10; i++)
    {
        auto start = steady_clock::now();
        auto final_board = cu_play();
        auto duration = duration_cast<ms>(steady_clock::now() - start);

        std::cout << std::dec << duration.count() << " ms\n";
        std::cout << std::hex << final_board << "\n";
        std::cout << std::dec << simulator::score(final_board) << "\n";
    }

    // cudaDeviceReset must be called before exiting in order for profiling and
    // tracing tools such as Nsight and Visual Profiler to show complete traces
    cudaDeviceReset();
}