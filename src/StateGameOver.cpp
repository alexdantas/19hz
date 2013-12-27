#include "StateGameOver.hpp"
#include "InputManager.hpp"

GAMESTATE_DEF(StateGameOver)

StateGameOver::StateGameOver(ArgsBase* args) {
	bg = new Sprite("img/gameover.png");
	InputManager::instance()->connect(InputManager::KEYDOWN, this, &StateGameOver::handleKeyDown);
}

StateGameOver::~StateGameOver() {
	delete bg;
}

void StateGameOver::update() {
	
}

void StateGameOver::render() {
	bg->render();
}

void StateGameOver::handleKeyDown(const observer::Event& event, bool& stop) {
	if (inputmanager_event.key.keysym.sym == SDLK_RETURN || inputmanager_event.key.keysym.sym == SDLK_KP_ENTER)
		throw new Change("StateFirst");
}
