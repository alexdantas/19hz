#include "State.hpp"

#include "InputManager.hpp"

using namespace common;

using std::string;
using std::map;
using std::list;

// =============================================================================
// State::ArgsBase Class
// =============================================================================

State::ArgsBase::ArgsBase() {}
State::ArgsBase::~ArgsBase() {}

// =============================================================================
// State::Args Class
// =============================================================================

State::Change::Change(const string& state, ArgsBase* args) :
mexception("State::Change"), state_(state), args_(args) {}
State::Change::~Change() throw() {}

const string& State::Change::state() const { return state_; }
State::ArgsBase* State::Change::args() const { return args_; }

// =============================================================================
// State::StackUp Class
// =============================================================================

State::StackUp::StackUp(const string& state, ArgsBase* args) :
mexception("State::StackUp"), state_(state), args_(args) {}
State::StackUp::~StackUp() throw() {}

const string& State::StackUp::state() const { return state_; }
State::ArgsBase* State::StackUp::args() const { return args_; }

// =============================================================================
// State::Unstack Class
// =============================================================================

State::Unstack::Unstack(ArgsBase* args) : mexception("State::Unstack"), args_(args) {}
State::Unstack::~Unstack() throw() {}

State::ArgsBase* State::Unstack::args() const { return args_; }

// =============================================================================
// State Class
// =============================================================================

// Static vars
map<string, State::Builder> State::builders;
list<State*> State::states;

State::State() : frozen_(false) {
	InputManager::instance()->connect(InputManager::QUIT, this, &State::handleQuit);
}

State::~State() {
	InputManager::instance()->disconnect(this);
}

map<string, State::Builder>& State::getMap() {
	return *(*initMap());
}

void State::closeMap() {
	map<string, Builder>** init_map;
	try {
		init_map = initMap();
	}
	catch (mexception&) {
		throw mexception("Trying to close the initialization map of game states builders after engine initialization");
	}
	
	// assign initialization map to static map
	builders = *(*init_map);
	
	// closing initialization map
	delete *init_map;
	*init_map = 0;
}

map<string, State::Builder>** State::initMap() {
	static map<string, Builder>* init_map = new map<string, Builder>();
	
	// if the map was already closed (we're not in the static initialization anymore)
	if (!init_map)
		throw mexception("Trying to get the initialization map of game states builders after static initialization");
	
	return &init_map;
}

State* State::build(const string& name, ArgsBase* args) {
	// for unknown game states
	if (builders.find(name) == builders.end())
		throw mexception("Trying to load unknown game state");
	
	return (*builders[name])(args);
}

State::Builder State::getIdByName(const string& name) {
	// for unknown game states
	if (builders.find(name) == builders.end())
		throw mexception("Trying to get id of unknown game state");
	
	return builders[name];
}

void State::handleUnstack(ArgsBase* args) {}

bool State::frozen() const { return frozen_; }

void State::update() {}
void State::render() {}

void State::handleQuit(const observer::Event& event, bool& stop) {
	throw Quit();
}
