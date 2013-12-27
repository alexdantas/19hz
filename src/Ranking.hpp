
#ifndef RANKING_HPP
#define RANKING_HPP

#include <list>
#include <string>

#include "Text.hpp"

class Ranking {
private:
	class Record;
	
	std::list<Record*> records;
	
	Text* text;
public:
	Ranking(bool rendering = true);
	~Ranking();
	
	static void save(const std::string& player_name, int points);
	
	void render(int x = 0, int y = 0);
private:
	void insert(Record* record);
	void sort();
	void write();
};

#endif
