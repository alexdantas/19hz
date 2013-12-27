#ifndef TIMER_HPP
#define TIMER_HPP

#include "observer.hpp"

class Timer {
SUBJECT
public:
	enum {
		DONE = 0,
		
		// don't change this
		LASTEVENT
	};
private:
	bool done;
	bool paused;
	int done_ticks;
	int pausetime;
public:
	Timer();
	~Timer();
	
	void update();
	
	void start(int ms);
	void pause();
	void resume();
	void cancel();
	
	int time() const;
	bool ispaused() const;
private:
	void handleObsStackPush(const observer::Event& event, bool& stop);
	void handleObsStackPop(const observer::Event& event, bool& stop);
};

#endif
