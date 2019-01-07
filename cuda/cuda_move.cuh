#pragma once
#include "cuda_runtime.h"
#include "device_launch_parameters.h"

#include <stdint.h>
#include <memory>
#include <vector>

#include "game.h"

namespace cuda_sim 
{
    namespace cumove
    {
        __device__ uint64_t place_random(uint64_t board);
        __device__ uint64_t do_move(uint64_t board, int move);

        __device__ uint64_t board_right(uint64_t board);
        __device__ uint64_t board_left(uint64_t board);
        __device__ uint64_t board_up(uint64_t board);
        __device__ uint64_t board_down(uint64_t board);
    }
    
    void init();

    __global__ void move_kernel(const uint64_t* board, int* moves, uint64_t* moved_boards);

    template<int grid_size, int block_size>
    void move(
        const uint64_t* boards, const int*  moves, uint64_t* moved_boards)
    {
        const int N = block_size * grid_size;

        int* d_moves = nullptr;
        uint64_t* d_boards = nullptr;
        uint64_t* d_moved_boards = nullptr;

        cudaMalloc(&d_moves, N * sizeof(int));
        cudaMalloc(&d_boards, N * sizeof(uint64_t));
        cudaMalloc(&d_moved_boards, N * sizeof(uint64_t));

        cudaMemcpy(d_moves, moves, N * sizeof(int), cudaMemcpyHostToDevice);
        cudaMemcpy(d_boards, boards, N * sizeof(uint64_t), cudaMemcpyHostToDevice);

        cuda_sim::move_kernel << <grid_size, block_size >> >(d_boards, d_moves, d_moved_boards);

        cudaMemcpy(moved_boards, d_moved_boards, N * sizeof(uint64_t), cudaMemcpyDeviceToHost);

        cudaFree(d_moves);
        cudaFree(d_boards);
        cudaFree(d_moved_boards);
    }

    __global__ void new_board_kernel(uint64_t* boards);

    template<int grid_size, int block_size>
    void new_board(uint64_t* new_boards)
    {
        uint64_t* d_boards = nullptr;
        const int N = block_size * grid_size;

        cudaMalloc(&d_boards, N * sizeof(uint64_t));
        cuda_sim::new_board_kernel << <grid_size, block_size >> >(d_boards);
        cudaMemcpy(new_boards, d_boards, N * sizeof(uint64_t), cudaMemcpyDeviceToHost);

        cudaFree(d_boards);
    }

    __global__ void random_move_kernel(uint64_t* boards, const int stop);
 
    template<int grid_size, int block_size, int stop>
    void random_move(uint64_t* boards)
    {
        uint64_t* d_boards = nullptr;
        const int N = block_size * grid_size;

        cudaMalloc(&d_boards,       N * sizeof(uint64_t));

        cudaMemcpy(d_boards, boards, N * sizeof(uint64_t), cudaMemcpyHostToDevice);
        
        cuda_sim::random_move_kernel << <grid_size, block_size >> >(d_boards, stop);
        
        cudaMemcpy(boards, d_boards, N * sizeof(uint64_t), cudaMemcpyDeviceToHost);
        
        cudaFree(d_boards);
    }

    __global__ void reduce_score_kernel(uint64_t* boards, uint64_t *output);
    
    template<int grid_size, int block_size>
    uint64_t get_score(const uint64_t * boards)
    {
        const int N = grid_size * block_size;
        int smem_size = block_size * sizeof(uint64_t);

        uint64_t* d_boards = nullptr;
        uint64_t* d_out = nullptr;

        cudaMalloc(&d_boards, N * sizeof(uint64_t));
        cudaMalloc(&d_out,    N * sizeof(uint64_t));

        cudaMemcpy(d_boards, boards, N * sizeof(uint64_t), cudaMemcpyHostToDevice);
        reduce_score_kernel << < grid_size, block_size, smem_size >> >(d_boards, d_out);

        auto output = std::make_unique<uint64_t[]>(N);
        cudaMemcpy(output.get(), d_out, N * sizeof(uint64_t), cudaMemcpyDeviceToHost);

        cudaFree(d_boards);
        cudaFree(d_out);

        uint64_t res = 0;
        for (int i = 0; i<grid_size; i++)
        {
            res += output[i];
        }

        return res;
    }

    int search_move(const uint64_t board);
    int search_move(const std::vector<std::vector<int>>& vec_board);
}