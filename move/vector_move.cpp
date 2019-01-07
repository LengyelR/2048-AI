#include <iostream>
#include <thread>
#include  <omp.h>

#include "vector_move.h"


namespace move {

	template<int direction>
	void merge(std::vector<int>& row);

	template<int start, class StopCondition, class Step, class ErrorPredicate>
	void _merge(std::vector<int>& row, StopCondition stop, Step step, ErrorPredicate error)
	{
		int i = start;

		for (; stop(i); i = step(i)) {

			if (row[i] == row[step(i)]) { // x x 2 2
				row[i] == 0 ? 0 : ++row[i];

				row[step(i)] = 0;
				i = step(i);
			}
			else if (row[step(i, 1)] == 0) { // x x 0 2
				if (error(step(i, 2))) return;
				if (row[i] == row[step(i, 2)]) { // x 2 0 2
					row[i] == 0 ? 0 : ++row[i];

					row[step(i, 2)] = 0;
					return;
				}
				else if (row[step(i, 2)] == 0) { // x 0 0 2
					if (error(step(i, 3))) return;
					if (row[i] == row[step(i, 3)]) { // 2 0 0 2
						row[i] == 0 ? 0 : ++row[i];

						row[step(i, 3)] = 0;
						return;
					}
				}
			}
		}
	}



	template<int direction>
	void compress(std::vector<int>& row);

	template<int start, class Step, class StopCondition>
	void _compress(std::vector<int>& row, Step step, StopCondition predicate)
	{
		std::vector<int> temp = { 0, 0, 0, 0 };
		int tidx = start;
		int ridx = start;

		for (; predicate(ridx); ridx = step(ridx))
		{
			if (row[ridx] != 0) {
				temp[tidx] = row[ridx];
				tidx = step(tidx);
			}
		}

		row.swap(temp);
	}




	template<>
	void merge<1>(std::vector<int>& row)
	{
		_merge<3>(row,
			[](int i) {return 0 < i;},
			[](int a, int b = 1) { return a - b;},
			[](int i) {return i < 0; });
	}
	template<>
	void merge<-1>(std::vector<int>& row)
	{
		_merge<0>(row,
			[](int i) {return i < 3;},
			[](int a, int b = 1) {return a + b;},
			[](int i) {return i >= 4; });
	}

	template<>
	void compress<-1>(std::vector<int>& row) {
		_compress<0>(row,
			[](int idx) {return ++idx;},
			[](int idx) {return idx <= 3;}
		);
	};

	template<>
	void compress<1>(std::vector<int>& row) {
		_compress<3>(row,
			[](int idx) {return --idx;},
			[](int idx) {return 0 <= idx;}
		);
	};


	void move_row_right(std::vector<int>& row)
	{
		merge<1>(row);
		compress<1>(row);
	}

	void move_row_left(std::vector<int>& row)
	{
		merge<-1>(row);
		compress<-1>(row);
	}

	void move_mtx_right(std::vector<int>& row0,
		std::vector<int>& row1,
		std::vector<int>& row2,
		std::vector<int>& row3)
	{
		move_row_left(row0);
		move_row_left(row1);
		move_row_left(row2);
		move_row_left(row3);
	}


	void move_mtx_left(std::vector<int>& row0,
		std::vector<int>& row1,
		std::vector<int>& row2,
		std::vector<int>& row3)
	{
		#pragma omp parallel num_threads(4)
		{
			int num = omp_get_thread_num();

			if (num == 0)       move_row_left(row0);
			else if (num == 1)  move_row_left(row1);
			else if (num == 2)  move_row_left(row2);
			else if (num == 3)  move_row_left(row3);
		}
	}

}