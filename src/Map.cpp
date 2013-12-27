#include <cstdio>

#include "Map.hpp"

#include "common.hpp"

using std::fstream;

void Map::load(const std::string& filename) {
	FILE* fp = fopen(common::RootPath::get(filename).c_str(), "r");
	int value;
	
	for (int i = 0; i < 22; ++i) {
		for (int j = 0; j < 40; ++j) {
			fscanf(fp, "%d", &value);
			fgetc(fp);
			switch (value) {
			case 0:
				beg_row = i;
				beg_col = j;
				break;
				
			case 1:
				matrix[i][j] = true;
				break;
				
			case 2:
				matrix[i][j] = false;
				break;
				
			case 3:
				end_row = i;
				end_col = j;
				break;
				
			default:
				break;
			}
		}
		fgetc(fp);
	}
	
	fclose(fp);
}
