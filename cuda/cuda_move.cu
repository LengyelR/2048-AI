#include "cuda_move.cuh"  // TODO: cuda.h, cuda_runtime.h, device_launch_parameters.h, etc...
#include "bit_move.h"

#include "cuda.h"
#include "curand.h"
#include "curand_kernel.h"
#include "cooperative_groups.h"

#define GET_CELL(board, idx) (board & cell_masks[idx]) >> (15 - idx) * 4

namespace cg = cooperative_groups;

namespace cuda_sim 
{

    int search_move(const uint64_t board)
    {
        //hard coded params, cython doesn't know template args
        const int stop = 300;
        const int grid_size = 2048;
        const int block_size = 512;
        const int N = grid_size * block_size;

        auto boards = std::make_unique<uint64_t[]>(N);

        uint64_t current_score = 0;
        uint64_t best_score = 0;
        int best_move = 0;

        //note: streaming doesn't help because this already achieves near 100% occupancy...
        for (int move = 0; move < 4; ++move)
        {
            auto moved_board = simulator::do_move(board, move);
            std::fill_n(boards.get(), N, moved_board);

            random_move<grid_size, block_size, stop>(boards.get());
            current_score = get_score<grid_size, block_size>(boards.get());

            if (best_score < current_score)
            {
                best_score = current_score;
                best_move = move;
            }
        }

        return best_move;
    }

    int search_move(const std::vector<std::vector<int>>& vec_board)
    {
        auto board = convert::to_bits(vec_board[0], vec_board[1], vec_board[2], vec_board[3]);
        return search_move(board);
    }

    __device__ const uint64_t cell_masks[16]
    {
        0xf000000000000000,
        0x0f00000000000000,
        0x00f0000000000000,
        0x000f000000000000,
        0x0000f00000000000,
        0x00000f0000000000,
        0x000000f000000000,
        0x0000000f00000000,
        0x00000000f0000000,
        0x000000000f000000,
        0x0000000000f00000,
        0x00000000000f0000,
        0x000000000000f000,
        0x0000000000000f00,
        0x00000000000000f0,
        0x000000000000000f
    };

    namespace utils
    {
        // could have used intrinsics for these 2
        // TODO: does it work properly for 16 bits?
        static __device__ __inline__ uint32_t __popcnt(const uint32_t x)
        {
            uint32_t count;
            asm volatile("popc.b32 %0, %1;" : "=r"(count) : "r"(x));
            return count;
        }

        static __device__ __inline__ uint32_t __myclock()
        {
            uint32_t mclk;
            asm volatile("mov.u32 %0, %%clock;" : "=r"(mclk));
            return mclk;
        }

        /// draws random number in range [1, upper_bound] (both inclusive)
        __device__ int get_random_int(curandState* state, int upper_bound)
        {
            curand_init(__myclock(), 0, 0, state);
            auto rand = curand_uniform(state);
            return static_cast<int>(rand*upper_bound + 0.999999);
        }
    }

    namespace cumove
    {
        __device__ int get_tile_position(uint16_t zero_cells, int rand_idx)
        {
            int counter = 0;
            for (int i = 0; i < 16; ++i)
            {
                if (zero_cells & (1 << i))
                {
                    counter++;
                }

                if (counter == rand_idx)
                {
                    return i;
                }
            }

            return -1;
        }

        __device__ uint64_t place_random(uint64_t board)
        {
            //todo: any faster?
            //uint16_t zero_cells = 0;
            //#pragma unroll  
            //for (int i = 0; i < 16; ++i)
            //{
            //    zero_cells |= (1 << i) * !!(board & cell_masks[i]);
            //}
            //zero_cells = ~zero_cells;

            uint16_t zero_cells = 0;
            for (int i = 0; i < 16; ++i)
            {
                if (board & cell_masks[i])
                {
                    continue;
                }
                else
                {
                    zero_cells |= 1 << i;
                }
            }

            if (zero_cells == 0)
            {
                return board;
            }

            int range_end = utils::__popcnt(zero_cells);

            curandState state;
            auto rand_idx = utils::get_random_int(&state, range_end);

            auto idx = get_tile_position(zero_cells, rand_idx);
            auto new_cell = curand_uniform(&state) > 0.9 ? 2ULL : 1ULL;

            return board | (new_cell << (60 - 4 * idx));
        }

        __device__ static uint16_t row_right[0xffff];
        __device__ static uint16_t row_left[0xffff];

        const uint64_t ROW_MASK0 = 0xffff000000000000;
        const uint64_t ROW_MASK1 = 0x0000ffff00000000;
        const uint64_t ROW_MASK2 = 0x00000000ffff0000;
        const uint64_t ROW_MASK3 = 0x000000000000ffff;

        __device__ uint64_t do_move(uint64_t board, int move)
        {
            switch (move)
            {
            case 0:
                board = board_up(board);
                break;
            case 1:
                board = board_down(board);
                break;
            case 2:
                board = board_left(board);
                break;
            case 3:
                board = board_right(board);
                break;
            }

            return board;
        }

        //todo: make it __host__ __device__ 
        __device__ uint64_t transpose(uint64_t x)
        {
            uint64_t a1 = x & 0xf0f00f0ff0f00f0fULL;
            uint64_t a2 = x & 0x0000f0f00000f0f0ULL;
            uint64_t a3 = x & 0x0f0f00000f0f0000ULL;
            uint64_t a = a1 | (a2 << 12) | (a3 >> 12);
            uint64_t b1 = a & 0xff00ff0000ff00ffULL;
            uint64_t b2 = a & 0x00ff00ff00000000ULL;
            uint64_t b3 = a & 0x00000000ff00ff00ULL;
            return b1 | (b2 >> 24) | (b3 << 24);
        }


        template<uint16_t row_direction[0xffff]>
        __device__ uint64_t board_move(uint64_t board)
        {
            uint16_t row0 = (board & ROW_MASK0) >> 48;
            uint16_t row1 = (board & ROW_MASK1) >> 32;
            uint16_t row2 = (board & ROW_MASK2) >> 16;
            uint16_t row3 = (board & ROW_MASK3);

            auto res0 = static_cast<uint64_t>(row_direction[row0]) << 48;
            auto res1 = static_cast<uint64_t>(row_direction[row1]) << 32;
            auto res2 = static_cast<uint64_t>(row_direction[row2]) << 16;
            auto res3 = static_cast<uint64_t>(row_direction[row3]);

            return res0 | res1 | res2 | res3;
        }

        __device__ uint64_t board_right(uint64_t board)
        {
            return board_move<row_right>(board);
        }

        __device__ uint64_t board_left(uint64_t board)
        {
            return board_move<row_left>(board);
        }

        __device__ uint64_t board_up(uint64_t board)
        {
            auto transposed = transpose(board);
            auto moved = board_left(transposed);
            return transpose(moved);
        }

        __device__ uint64_t board_down(uint64_t board)
        {
            auto transposed = transpose(board);
            auto moved = board_right(transposed);
            return transpose(moved);
        }

    }

    __device__ uint64_t score(uint64_t board)
    {
        uint64_t score = 0;
        for (int i = 0; i < 16; ++i)
        {
            auto cell = GET_CELL(board, i);
            score += ((2 << (cell - 1)) & ~1ULL);
        }
        return score;
    }

    void init()
    {
        move::init();
        cudaMemcpyToSymbol(cumove::row_right, move::row_right, 0xffff * sizeof(uint16_t));
        cudaMemcpyToSymbol(cumove::row_left, move::row_left, 0xffff * sizeof(uint16_t));
    }

    __global__ void move_kernel(const uint64_t* boards, int* moves, uint64_t* moved_boards)
    {
        int tidx = blockIdx.x * blockDim.x + threadIdx.x;
        auto moved_board = cumove::do_move(boards[tidx], moves[tidx]);
        moved_boards[tidx] = moved_board == boards[tidx] 
                                ? moved_board 
                                : cumove::place_random(moved_board);
    }

    __device__ uint64_t new_board()
    {
        auto new_board = cumove::place_random(0);
        return cumove::place_random(new_board);
    }
    __global__ void new_board_kernel(uint64_t* boards)
    {
        int tidx = blockIdx.x * blockDim.x + threadIdx.x;
        boards[tidx] = new_board();
    }
    
    __global__ void random_move_kernel(uint64_t* boards, const int stop)
    {
        curandState state;
        int tidx = blockIdx.x * blockDim.x + threadIdx.x;

        for (int i = 0; i < stop; i++)
        {
            auto copy = boards[tidx];

            auto rand_move = (utils::get_random_int(&state, 4)-1);
            auto moved_board = cumove::do_move(boards[tidx], rand_move);
            boards[tidx] = moved_board == copy
                ? moved_board
                : cumove::place_random(moved_board);
        }
    }

    __global__ void reduce_score_kernel(uint64_t* boards, uint64_t* output)
    {
        cg::thread_block cta = cg::this_thread_block();
        extern __shared__ uint64_t sdata[];

        unsigned int tid = threadIdx.x;
        unsigned int i = blockIdx.x*blockDim.x * 2 + threadIdx.x;
        unsigned int gridSize = blockDim.x * 2 * gridDim.x;
        unsigned int n = blockDim.x * gridDim.x;
        uint64_t mySum = 0;

        while (i < n)
        {
            mySum += score(boards[i]);
            mySum += score(boards[i + blockDim.x]);
            i += gridSize;
        }

        sdata[tid] = mySum;
        cg::sync(cta);

        if (tid < 256)
        {
            sdata[tid] = mySum = mySum + sdata[tid + 256];
        }
        cg::sync(cta);

        if (tid < 128)
        {
            sdata[tid] = mySum = mySum + sdata[tid + 128];
        }
        cg::sync(cta);

        if (tid <  64)
        {
            sdata[tid] = mySum = mySum + sdata[tid + 64];
        }
        cg::sync(cta);

        auto tile32 = cg::tiled_partition<32>(cta);
        if (cta.thread_rank() < 32)
        {
            mySum += sdata[tid + 32];
            for (int offset = tile32.size() / 2; offset > 0; offset /= 2)
            {
                mySum += tile32.shfl_down(mySum, offset);
            }
        }

        if (cta.thread_rank() == 0)
        {
            output[blockIdx.x] = mySum;
        }
    }
}