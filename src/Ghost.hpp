
#ifndef GHOST_HPP
#define GHOST_HPP

#include "GameObject.hpp"
#include "Timer.hpp"
#include "Stopwatch.hpp"

class Ghost : public GameObject {
private:
	lalge::R2Vector speed;
	Timer timer;
	Stopwatch stopwatch;
	lalge::R2Vector border_position;
public:
	static GameObject* player;
	Sprite* sprite_border;
	bool border_set;
	
	Ghost(Sprite* sprite, Sprite* sprite_border);
	
	void update();
	void render();
	void renderBorder();
private:
	void handleTimerDone(const observer::Event& event, bool& stop);
	void seek();
	void hitPlayer(lalge::Scalar dt);
	lalge::Scalar getAngle() const;
};

#endif
