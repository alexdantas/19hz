
/**

@file State.hpp
@author Matheus Pimenta

*/

#ifndef STATE_HPP
#define STATE_HPP

#include <string>
#include <map>

#include "common.hpp"
#include "observer.hpp"

#include "Text.hpp"
#include "Interaction.hpp"

/// Declares the game state ID.
#define GAMESTATE				\
public:							\
	class Assigner {			\
	public:						\
		Assigner();				\
	};							\
private:						\
	static Assigner assigner;	\
public:							\
	Builder id() const;

/// Defines the game state builder and ID.
#define GAMESTATE_DEF(GameState)														\
GameState::Assigner GameState::assigner;												\
State* build##GameState(State::ArgsBase* args) { return new GameState(args); }			\
GameState::Assigner::Assigner() { State::getMap()[#GameState] = &build##GameState; }	\
State::Builder GameState::id() const { return &build##GameState; }

/// Base class for a game state.
class State {
public:
	/// Class for passing arguments between game states.
	class ArgsBase {
	public:
		/// Empty constructor.
		ArgsBase();
		
		/// Empty destructor.
		virtual ~ArgsBase();
	};
	
	/// Throw (POINTER!) this class to change the current game state.
	class Change : public common::mexception {
	private:
		std::string state_;
		ArgsBase* args_;
	public:
		/// Assignmest constructor.
		Change(const std::string& state, ArgsBase* args = 0);
		
		/// Empty destructor.
		~Change() throw();
		
		/// Access method to the name of the new game state.
		const std::string& state() const;
		
		/// Access method to the arguments.
		ArgsBase* args() const;
	};
	
	/// Throw (POINTER!) this class to stack up the current game state and load a new one.
	class StackUp : public common::mexception {
	private:
		std::string state_;
		ArgsBase* args_;
	public:
		/// Assignmest constructor.
		StackUp(const std::string& state, ArgsBase* args = 0);
		
		/// Empty destructor.
		~StackUp() throw();
		
		/// Access method to the name of the new game state.
		const std::string& state() const;
		
		/// Access method to the arguments.
		ArgsBase* args() const;
	};
	
	/// Throw (POINTER!) this class if you want to unstack a game state.
	class Unstack : public common::mexception {
	private:
		ArgsBase* args_;
	public:
		/// Assignmest constructor.
		Unstack(ArgsBase* args = 0);
		
		/// Empty destructor.
		~Unstack() throw();
		
		/// Access method to the arguments.
		ArgsBase* args() const;
	};
	
	/// Game state builder definition.
	typedef State* (*Builder)(ArgsBase*);
private:
	/// Map with all game state builders.
	static std::map<std::string, Builder> builders;
public:
	/// List with all loaded game states.
	static std::list<State*> states;
protected:
	bool frozen_;
public:
	/// Empty constructor.
	State();
	
	/// Empty destructor.
	virtual ~State();
	
	static std::map<std::string, Builder>& getMap();
	static void closeMap();
private:
	static std::map<std::string, Builder>** initMap();
public:
	static State* build(const std::string& name, ArgsBase* args = 0);
	static Builder getIdByName(const std::string& name);
	
	virtual void handleUnstack(ArgsBase* args);
	
	/// Access method to the game state ID.
	virtual Builder id() const = 0;
	
	/// The state tells if it's frozen, or not.
	bool frozen() const;
	
	virtual void update();
	virtual void render();
	
	virtual void handleQuit(const observer::Event& event, bool& stop);
};

#endif
