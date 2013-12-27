
#ifndef STATEFIRST_HPP
#define STATEFIRST_HPP

#include <list>

#include "State.hpp"
#include "Player.hpp"
#include "Map.hpp"
#include "Ghost.hpp"
#include "Animation.hpp"
#include "Timer.hpp"
#include "Stopwatch.hpp"
#include "Audio.hpp"

class StateFirst : public State {
GAMESTATE
private:
	Sprite* bg;
	Sprite* spr_ghost;
	Sprite* spr_ghost_border;
	Sprite* spr_player_vision;
	Sprite* spr_player;
	Animation* anim_player;
	Animation* anim_player_heart;
	Player* player;
	Map* map;
	std::list<Ghost*> ghosts;
	Timer timer;
	Stopwatch stopwatch;
	Audio* heart_beats[4];
	static int level;
public:
	StateFirst(ArgsBase* args);
	~StateFirst();
	
	void update();
	void render();
private:
	void renderBorders();
	void handleTimerDone(const observer::Event& event, bool& stop);
};

#endif
