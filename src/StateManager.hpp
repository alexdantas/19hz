
#ifndef STATEMANAGER_HPP
#define STATEMANAGER_HPP

#include "common.hpp"

#include "Text.hpp"

/// Class to manage all game states.
class StateManager {
private:
	Text* fps;
public:
	StateManager();		///< Initializes SDL and other stuff.
	~StateManager();	///< Closes everything.
private:
	void initStuff();
	void loadFirst();
	
	void dumpStates();
	void closeStuff();
public:
	void run();	///< Method to run the game.
private:
	void input();
	void update();
	void render();
	
	void renderFPS();
	
	void handleMexception(common::mexception* e);
};

#endif
