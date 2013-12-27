#include "SDL.h"

#include "observer.hpp"

#include "Timer.hpp"

Timer::Timer() : done(true), paused(false) {
	subject.init(LASTEVENT);
	observer::Stack::connect(this, &Timer::handleObsStackPush, &Timer::handleObsStackPop);
}

Timer::~Timer() {
	observer::Stack::disconnect(this);
}

void Timer::update() {
	if ((!done) && (!paused) && (SDL_GetTicks() >= done_ticks)) {
		done = true;
		subject.broadcast(observer::Event(DONE));
	}
}

void Timer::start(int ms) {
	if (ms > 0) {
		done_ticks = ms + SDL_GetTicks();
		done = false;
		paused = false;
	}
}

void Timer::pause() {
	if (!paused) {
		pausetime = SDL_GetTicks();
		paused = true;
	}
}

void Timer::resume() {
	if (paused) {
		done_ticks += (SDL_GetTicks() - pausetime);
		paused = false;
	}
}

void Timer::cancel() { done = true; }

int Timer::time() const {
	if (done)
		return 0;
	
	if (paused)
		return (done_ticks - pausetime);
	
	return (done_ticks - SDL_GetTicks());
}

bool Timer::ispaused() const { return paused; }

void Timer::handleObsStackPush(const observer::Event& event, bool& stop) { pause(); }
void Timer::handleObsStackPop(const observer::Event& event, bool& stop) { resume(); }
