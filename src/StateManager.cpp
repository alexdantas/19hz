
#if (defined _WIN32 || defined __WIN32__ || defined __CYGWIN32__) && defined RELEASE
	#define _WIN32_WINNT	0x0500
	#include <windows.h>
#endif

#include "StateManager.hpp"

#include "SDLBase.hpp"
#include "InputManager.hpp"
#include "SurfaceManager.hpp"
#include "StateMovie.hpp"
#include "Button.hpp"
#include "GameBGM.hpp"

#ifndef RELEASE
	#define RELEASE	""
#endif

using namespace common;

using std::string;
using std::list;
using std::cout;

StateManager::StateManager() : fps(0) {
	// hide console in windows
#ifdef _WIN32_WINNT
	ShowWindow(GetConsoleWindow(), SW_HIDE);
#endif
	
	SDLBase::init();
	InputManager::instance();
	SurfaceManager::instance();
	initStuff();
	loadFirst();
}

StateManager::~StateManager() {
	dumpStates();
	closeStuff();
	SurfaceManager::close();
	InputManager::close();
	SDLBase::close();
}

void StateManager::initStuff() {
	// initializes the root path
	string path = MainArgs::get<string>("-p");
	if (!path.size())
		path = MainArgs::get<string>("--path");
	RootPath::init(path);
	
	// initializes the fps
	if ((MainArgs::find("-f")) || (MainArgs::find("--fps")))
		fps = new Text("", "", 20, 0, SDLBase::getColor(255, 255, 255), Text::shaded, SDLBase::getColor(0, 0, 0));
	
	// game BGM
	GameBGM::init();
	
	// button sounds
	Button::sound_hover = new Audio("sfx/button_hover.wav");
	Button::sound_clicked = new Audio("sfx/button_clicked.wav");
}

void StateManager::loadFirst() {
	// closing the initialization map of game states builders
	State::closeMap();
	
	// release version
	if (string(RELEASE).size()) {
		State::states.push_back(State::build(RELEASE));
		return;
	}
	
	// searches for the game state to be loaded
	string state = MainArgs::get<string>("-s");
	if (!state.size())
		state = MainArgs::get<string>("--state");
	// for a movie as the first game state
	if (state == "StateMovie") {
		string movie = MainArgs::get<string>(state);
		string nextstate = MainArgs::get<string>(movie);
		State::states.push_back(new StateMovie(new StateMovie::Args(movie, nextstate)));
	}
	// you can't load pause manager as your first game state
	else if (state == "StatePauseManager")
		throw mexception("Trying to load pause manager as the first game state");
	// for specific game states
	else
		State::states.push_back(State::build(state));
}

void StateManager::dumpStates() {
	// to dump this list we must delete the objects
	while (State::states.size()) {
		delete State::states.back();
		State::states.pop_back();
	}
}

void StateManager::closeStuff() {
	// closes the fps text
	if (fps)
		delete fps;
	
	// game BGM
	GameBGM::close();
	
	// button sounds
	delete Button::sound_hover;
	delete Button::sound_clicked;
}

void StateManager::run() {
	// the main game loop
	while (true) {
		SDLBase::delayFrame();
		try {
			input();
			update();
			render();
		}
		catch (mexception* e) {
			handleMexception(e);
		}
	}
}

void StateManager::input() {
	InputManager::instance()->update();
}

void StateManager::update() {
	// for all loaded states
	for (list<State*>::iterator it = State::states.begin(); it != State::states.end(); ++it) {
		// update if not frozen
		if (!(*it)->frozen())
			(*it)->update();
	}
}

void StateManager::render() {
	// for all the other loaded states
	for (list<State*>::iterator it = State::states.begin(); it != State::states.end(); ++it) {
		(*it)->render();
		// render stack screen if frozen
		if ((*it)->frozen())
			SDLBase::renderStackScreen();
	}
	
	renderFPS();
	
	SDLBase::updateScreen();
}

void StateManager::renderFPS() {
	if (!fps)
		return;
	
	char tmp[15];
	sprintf(tmp, "FPS: %.1f", SDLBase::FPS());
	
	fps->setText(string(tmp));
	fps->render(SDLBase::screen()->w - fps->w(), 0, false);
}

void StateManager::handleMexception(mexception* e) {
	if (!e)
		return;
	
	try {
		string what = e->what();
		if (what == "Quit") {
			// So you wanna quit? You don't say... DON'T THROW POINTERS TO QUIT REQUESTS!
			cout << "So you wanna quit? You don't say... DON'T THROW POINTERS TO QUIT REQUESTS!\n";
			delete e;
			throw Quit();
		}
		else if (what == "State::StackUp")
			throw (State::StackUp*)e;
		else if (what == "State::Unstack")
			throw (State::Unstack*)e;
		else if (what == "State::Change")
			throw (State::Change*)e;
		
		// So it was an error? You don't say... DON'T THROW POINTERS TO ERRORS!
		cout << "So it was an error? You don't say... DON'T THROW POINTERS TO ERRORS!\n";
		delete e;
		throw mexception(what);
	}
	// push the new game state
	catch (State::StackUp* su) {
		observer::Stack::push();
		State::states.push_back(State::build(su->state(), su->args()));
		delete e;
	}
	// maybe unstack a game state
	catch (State::Unstack* us) {
		// throw exception if the list is empty
		if (State::states.size() == 1)
			throw mexception("Trying to drop all game states");
		// unstack a game state
		delete State::states.back();
		State::states.pop_back();
		observer::Stack::pop();
		// handle the unstack args
		State::ArgsBase* args = us->args();
		delete e;
		try {
			State::states.back()->handleUnstack(args);
		}
		// handle again if needed
		catch (mexception* us_e) {
			handleMexception(us_e);
		}
	}
	// change the last pointer
	catch (State::Change* ch) {
		delete State::states.back();
		State::states.back() = State::build(ch->state(), ch->args());
		delete e;
	}
}
