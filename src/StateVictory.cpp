#include "StateVictory.hpp"
#include "InputManager.hpp"

GAMESTATE_DEF(StateVictory)

StateVictory::StateVictory(ArgsBase* args) {
	bg = new Sprite("img/victory.png");
	InputManager::instance()->connect(InputManager::KEYDOWN, this, &StateVictory::handleKeyDown);
}

StateVictory::~StateVictory() {
	delete bg;
}

void StateVictory::update() {
	
}

void StateVictory::render() {
	bg->render();
}

void StateVictory::handleKeyDown(const observer::Event& event, bool& stop) {
	if (inputmanager_event.key.keysym.sym == SDLK_RETURN || inputmanager_event.key.keysym.sym == SDLK_KP_ENTER)
		throw new Change("StateFirst");
}
