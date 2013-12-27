#include "Ghost.hpp"

#include "Rectangle.hpp"
#include "SDLBase.hpp"
#include "Player.hpp"

using namespace lalge;

#define SEEK_DELAY	4000
#define SPEED		100
#define SPEED_RANGE	100
#define DAMAGE		5
#define HIT_DELAY	1000

GameObject* Ghost::player;

Ghost::Ghost(Sprite* sprite, Sprite* sprite_border) : border_set(false) {
	setShape(new Rectangle());
	this->sprite = sprite;
	this->sprite_border = sprite_border;
	getShape()->position = r2vec(rand()%700, rand()%700);
	((Rectangle*)getShape())->setWidth(sprite->rectW());
	((Rectangle*)getShape())->setHeight(sprite->rectH());
	
	timer.connect(Timer::DONE, this, &Ghost::handleTimerDone);
	stopwatch.start();
	seek();
}

void Ghost::update() {
	Scalar dt = Scalar(SDLBase::dt())/1000;
	
	timer.update();
	hitPlayer(dt);
	getShape()->position += speed*dt;
}

void Ghost::render() {
	if (speed.length())
		sprite->rotozoom(getAngle());
	GameObject::render();
}

void Ghost::renderBorder() {
	if (!border_set) {
		border_position = getShape()->position;
		border_set = true;
	}
	
	Sprite* tmp = sprite;
	R2Vector tmp_vec = getShape()->position;
	sprite = sprite_border;
	getShape()->position = border_position;
	if (speed.length())
		sprite->rotozoom(getAngle());
	GameObject::render();
	sprite = tmp;
	getShape()->position = tmp_vec;
}

void Ghost::handleTimerDone(const observer::Event& event, bool& stop) {
	if (!player->hidden)
		seek();
}

void Ghost::seek() {
	timer.start(SEEK_DELAY);
	
	R2Vector range_vector = getShape()->range(*(player->getShape()));
	if (!range_vector.length())
		return;
	speed = range_vector.unitvec()*(SPEED + rand()%(SPEED_RANGE + 1) - SPEED_RANGE/2);
	rotate(speed, rand()%int(((Player*)player)->hp + 1) - int(((Player*)player)->hp)/2);
}

void Ghost::hitPlayer(Scalar dt) {
	if (player->hidden)
		return;
	
	if (stopwatch.time() > HIT_DELAY && ((Rectangle*)getShape())->collides(*((Rectangle*)player->getShape()))) {
		((Player*)player)->hp -= DAMAGE;
		stopwatch.start();
	}
	if (((Player*)player)->hp < 1)
		((Player*)player)->hp = 1;
}

Scalar Ghost::getAngle() const {
	Scalar angle_tmp = r2vec(0, -1)%speed;
	if (speed.x(0) > 0) {
		if (speed.x(1) > 0)
			angle_tmp = 225;
		else if (speed.x(1) < 0)
			angle_tmp = 315;
		else
			angle_tmp = 270;
	}
	return angle_tmp + 90;
}
