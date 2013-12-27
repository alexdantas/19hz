#ifndef STATEMOVIE_HPP
#define STATEMOVIE_HPP

#include "State.hpp"
#include "Sprite.hpp"
#include "Audio.hpp"
#include "Stopwatch.hpp"

class StateMovie : public State {
GAMESTATE
public:
	class Args : public ArgsBase {
	public:
		std::string path;
		//int nextstate;
		bool pause_able;
		bool skip_able;
		int skip_fade;
		
		std::string movie;
		std::string nextstate;
		
		//Args(const std::string& name, int nextstate, bool pause_able = true, bool skip_able = true, int skip_fade = 1500);
		Args(const std::string& movie, const std::string& nextstate);
	};
private:
	int nextstate;
	bool pause_able;
	bool skip_able;
	int skip_fade;
	
	Sprite* frames;
	int* sizes;
	int amount;
	int index;
	
	Audio* bgm;
	
	Stopwatch stopwatch;
	Stopwatch skip_stopwatch;
public:
	StateMovie(ArgsBase* args);
	~StateMovie();
	
	void update();
	void render();
private:
	void handleKeyDown(const observer::Event& event, bool& stop);
	
	void loadData(const std::string& path);
};

#endif
