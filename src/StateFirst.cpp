#include <ctime>

#include "StateFirst.hpp"

#include "Rectangle.hpp"
#include "InputManager.hpp"

using namespace lalge;
using std::list;

#define SCREEN_CHANGE_DELAY	3000
#define BORDER_DELAY		4000
#define BORDER_SHOWING		250

GAMESTATE_DEF(StateFirst)

int StateFirst::level = 1;

std::string levelToString(int i) {
	switch (i) {
		case 1: return "1";
		case 2: return "2";
		case 3: return "3";
		case 4: return "4";
		default:
			break;
	}
}

StateFirst::StateFirst(ArgsBase* args) {
	srand(time(0));
	
	Audio::musicVolume(0.1);
	
	bg = new Sprite("img/fase" + levelToString(level) + ".png");
	spr_ghost = new Sprite("img/ghost.png");
	spr_ghost_border = new Sprite("img/ghost_border.png");
	spr_player_vision = new Sprite("img/player_vision.png");
	spr_player = new Sprite("img/player.png");
	anim_player = new Animation("img/player_walking.png", 0, 10, 8, 12, true);
	anim_player_heart = new Animation("img/player_heart.png", 0, 4, 1, 12);
	
	map = new Map();
	map->load("map/level" + levelToString(level) + ".txt");
	
	heart_beats[0] = new Audio("sfx/heart0.wav");
	heart_beats[1] = new Audio("sfx/heart1.wav");
	heart_beats[2] = new Audio("sfx/heart2.wav");
	heart_beats[3] = new Audio("sfx/heart3.wav");
	
	player = new Player();
	player->sprite_vision = spr_player_vision;
	player->sprite = spr_player;
	player->animation = anim_player;
	player->animation_heart = anim_player_heart;
	player->map = map;
	player->heart_beats = (Audio**)heart_beats;
	((Rectangle*)player->getShape())->position = r2vec(map->beg_col*32 + 16, map->beg_row*32 + 24);
	((Rectangle*)player->getShape())->setWidth(player->sprite->rectW());
	((Rectangle*)player->getShape())->setHeight(player->sprite->rectH());
	
	Ghost::player = player;
	int ghost_amount = rand()%5 + 3;
	for (int i = 0; i < ghost_amount; ++i)
		ghosts.push_back(new Ghost(spr_ghost, spr_ghost_border));
	
	timer.connect(Timer::DONE, this, &StateFirst::handleTimerDone);
	stopwatch.start();
}

StateFirst::~StateFirst() {
	while (ghosts.size()) {
		delete ghosts.back();
		ghosts.pop_back();
	}
	
	delete player;
	delete spr_ghost;
	delete spr_ghost_border;
	delete spr_player_vision;
	delete spr_player;
	delete anim_player;
	delete anim_player_heart;
	delete bg;
	delete map;
	for (int i = 0; i < 4; ++i)
		delete heart_beats[i];
}

void StateFirst::update() {
	player->update();
	for (list<Ghost*>::iterator it = ghosts.begin(); it != ghosts.end(); ++it)
		(*it)->update();
	if ((player->hp == 1 || (map->end_row == player->row && map->end_col == player->col)) && !timer.time())
		timer.start(SCREEN_CHANGE_DELAY);
	timer.update();
}

void StateFirst::render() {
	bg->render();
	for (list<Ghost*>::iterator it = ghosts.begin(); it != ghosts.end(); ++it)
		(*it)->render();
	player->render();
	renderBorders();
}

void StateFirst::renderBorders() {
	if (player->hidden)
		return;
	
	int stp_time = stopwatch.time();
	
	if (stp_time > BORDER_DELAY && stp_time < BORDER_DELAY + BORDER_SHOWING) {
		for (list<Ghost*>::iterator it = ghosts.begin(); it != ghosts.end(); ++it)
			(*it)->renderBorder();
	}
	else if (stp_time >= BORDER_DELAY + BORDER_SHOWING) {
		for (list<Ghost*>::iterator it = ghosts.begin(); it != ghosts.end(); ++it)
			(*it)->border_set = false;
		stopwatch.start();
	}
}

void StateFirst::handleTimerDone(const observer::Event& event, bool& stop) {
	if (player->hp == 1) {
		level = 1;
		throw new Change("StateGameOver");
	}
	else if (level < 3) {
		level++;
		throw new Change("StateFirst");
	}
	else {
		level = 1;
		throw new Change("StateVictory");
	}
}
