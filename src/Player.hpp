
#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "GameObject.hpp"
#include "Map.hpp"
#include "Animation.hpp"
#include "Audio.hpp"

class Player : public GameObject {
private:
	lalge::R2Vector speed_dir;
public:
	int row, col;
	lalge::Scalar hp;
	
	Sprite* sprite_vision;
	Animation* animation;
	Animation* animation_heart;
	Map* map;
	Audio** heart_beats;
	
	Player();
	
	void update();
	void render();
private:
	void updateSpeed();
	bool cantGo(int i, int j, lalge::Scalar dt) const;
	void updateVision(lalge::Scalar dt);
	void selectAnimationRow(lalge::Scalar angle);
	void updateHeartAnimation();
	void resolveCollision();
};

#endif
