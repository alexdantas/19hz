#ifndef STOPWATCH_HPP
#define STOPWATCH_HPP

#include "observer.hpp"

class Stopwatch {
private:
	int initialtime;
	int pausetime;
	bool paused;
public:
	Stopwatch();
	~Stopwatch();
	
	void start();
	void pause();
	void resume();
	
	int time() const;
	bool ispaused() const;
private:
	void handleObsStackPush(const observer::Event& event, bool& stop);
	void handleObsStackPop(const observer::Event& event, bool& stop);
};

#endif
