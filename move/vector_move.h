#pragma once
#include <vector>

namespace move {

	void move_row_right(std::vector<int>& row);
	void move_row_left(std::vector<int>& row);

	void move_mtx_right(std::vector<int>& row0,
		std::vector<int>& row1,
		std::vector<int>& row2,
		std::vector<int>& row3);
	void move_mtx_left(std::vector<int>& row0,
		std::vector<int>& row1,
		std::vector<int>& row2,
		std::vector<int>& row3);
}
