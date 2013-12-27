
#ifndef MAP_HPP
#define MAP_HPP

#include <string>

class Map {
public:
	int beg_row, beg_col;
	int end_row, end_col;
	bool matrix[22][40];
	
	void load(const std::string& filename);
};

#endif
