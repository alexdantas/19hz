
#ifndef STATEGAMEOVER_HPP
#define STATEGAMEOVER_HPP

#include "State.hpp"
#include "Sprite.hpp"

class StateGameOver : public State {
GAMESTATE
private:
	Sprite* bg;
public:
	StateGameOver(ArgsBase* args);
	~StateGameOver();
	
	void update();
	void render();
private:
	void handleKeyDown(const observer::Event& event, bool& stop);
};

#endif
