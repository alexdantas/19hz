#include "SDL.h"

#include "observer.hpp"

#include "Stopwatch.hpp"

Stopwatch::Stopwatch() : initialtime(-1), paused(false) {
	observer::Stack::connect(this, &Stopwatch::handleObsStackPush, &Stopwatch::handleObsStackPop);
}

Stopwatch::~Stopwatch() {
	observer::Stack::disconnect(this);
}

void Stopwatch::start()
{
	initialtime = SDL_GetTicks();
	paused = false;
}

void Stopwatch::pause()
{
	if( ( !paused ) && ( initialtime != -1 ) )
	{
		pausetime = SDL_GetTicks();
		paused = true;
	}
}

void Stopwatch::resume()
{
	if( paused )
	{
		initialtime += ( SDL_GetTicks() - pausetime );
		paused = false;
	}
}

int Stopwatch::time() const
{
	if( initialtime == -1 )
		return -1;
	else if( paused )
		return( pausetime - initialtime );
	
	return( SDL_GetTicks() - initialtime );
}

bool Stopwatch::ispaused() const { return paused; }

void Stopwatch::handleObsStackPush(const observer::Event& event, bool& stop) { pause(); }
void Stopwatch::handleObsStackPop(const observer::Event& event, bool& stop) { resume(); }
