
#ifndef STATEVICTORY_HPP
#define STATEVICTORY_HPP

#include "State.hpp"
#include "Sprite.hpp"

class StateVictory : public State {
GAMESTATE
private:
	Sprite* bg;
public:
	StateVictory(ArgsBase* args);
	~StateVictory();
	
	void update();
	void render();
private:
	void handleKeyDown(const observer::Event& event, bool& stop);
};

#endif
