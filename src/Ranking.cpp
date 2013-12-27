#include <fstream>

#include "Ranking.hpp"

#include "common.hpp"

#include "SDLBase.hpp"

#define RANKING_PATH		"bin/nanotrip.rnk"
#define RANKING_MAX_SLOTS	10

#define GAP_WIDTH	50
#define GAP_HEIGHT	10

using namespace common;

using std::string;
using std::list;
using std::fstream;

class Ranking::Record {
public:
	string player_name;
	int points;
	Record(const string& player_name = "", int points = 0);
};

Ranking::Record::Record(const string& player_name, int points) :
player_name(player_name),
points(points)
{}

Ranking::Ranking(bool rendering) : text(0) {
	fstream f(RootPath::get(RANKING_PATH).c_str(), fstream::binary | fstream::in);
	
	if (!f.is_open()) {
		for (int i = 0; i < RANKING_MAX_SLOTS; ++i)
			records.push_back(new Record());
		return;
	}
	
	for (int i = 0; i < RANKING_MAX_SLOTS; ++i) {
		Record* record = new Record();
		int size;
		char buf;
		
		f.read((char*)&size, sizeof(int));
		for (int j = 0; j < size; ++j) {
			f.read(&buf, sizeof(char));
			record->player_name += buf;
		}
		f.read((char*)&record->points, sizeof(int));
		
		records.push_back(record);
	}
	
	f.close();
	
	if (rendering)
		text = new Text("ttf/Swiss721BlackRoundedBT.ttf", "", 20, 0, SDLBase::getColor(255, 255, 255), Text::blended);
}

Ranking::~Ranking() {
	// free memory
	while (records.size()) {
		delete records.back();
		records.pop_back();
	}
	
	if (text)
		delete text;
}

void Ranking::save(const string& player_name, int points) {
	Ranking data(false);
	Record* record = new Record(player_name, points);
	
	data.insert(record);
	data.sort();
	data.write();
}

void Ranking::render(int x, int y) {
	int point_max_w = 0;
	int max_w = 0;
	int max_h = 0;
	int w, h;
	int i;
	
	// calc max w h
	for (list<Record*>::iterator it = records.begin(); it != records.end(); ++it) {
		if (!(*it)->points)
			continue;
		
		text->calcSize((*it)->player_name, w, h);
		
		if (w > max_w)
			max_w = w;
		if (h > max_h)
			max_h = h;
		
		text->calcSize(eval((*it)->points), w, h);
		
		if (w > point_max_w)
			point_max_w = w;
	}
	
	if ((!max_w) && (!point_max_w))
		return;
	
	x -= (max_w + GAP_WIDTH + point_max_w)/2;
	y -= (max_h + GAP_HEIGHT)*RANKING_MAX_SLOTS/2;
	
	// render all
	i = 0;
	for (list<Record*>::iterator it = records.begin(); it != records.end(); ++it) {
		if (!(*it)->points)
			continue;
		
		text->setText((*it)->player_name);
		text->render(x, y + i*(max_h + GAP_HEIGHT), false);
		text->setText(eval((*it)->points));
		text->render(x + max_w + GAP_WIDTH, y + i*(max_h + GAP_HEIGHT), false);
		
		++i;
	}
}

void Ranking::insert(Record* record) {
	list<Record*>::iterator it = records.begin();
	
	// looks for existing name
	while ((it != records.end()) && ((*it)->player_name != record->player_name))
		++it;
	if (it != records.end()) {
		if ((*it)->points < record->points)
			(*it)->points = record->points;
		return;
	}
	
	// name not found. checks if the record points can be put in the ranking
	it = records.begin();
	while ((it != records.end()) && ((*it)->points > record->points))
		++it;
	if (it == records.end())
		delete record;
	else
		records.insert(it, record);
}

// bubble sort
void Ranking::sort() {
	bool swapped = true;
	
	while (swapped) {
		swapped = false;
		
		list<Record*>::iterator it1 = records.begin();
		list<Record*>::iterator it2;
		for (int i = 0; i < records.size() - 1; ++i) {
			it2 = it1;
			++it2;
			
			if ((*it1)->points < (*it2)->points) {
				swapped = true;
				
				Record* tmp = *it1;
				*it1 = *it2;
				*it2 = tmp;
			}
			
			++it1;
		}
	}
}

void Ranking::write() {
	fstream f(RootPath::get(RANKING_PATH).c_str(), fstream::binary | fstream::out);
	
	while (records.size()) {
		Record* record = records.front();
		int size = record->player_name.size();
		
		f.write((const char*)&size, sizeof(int));
		f.write(record->player_name.c_str(), size);
		f.write((const char*)&record->points, sizeof(int));
		
		delete record;
		records.pop_front();
	}
	
	f.close();
}
