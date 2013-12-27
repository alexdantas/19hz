#include "Player.hpp"

#include "Rectangle.hpp"
#include "SDLBase.hpp"
#include "InputManager.hpp"

using namespace lalge;

#define SPEED_ABS			200
#define MAX_FREQUENCY		15

Player::Player() : hp(100) {
	setShape(new Rectangle());
}

void Player::update() {
	Scalar dt = Scalar(SDLBase::dt())/1000;
	
	if (hp == 1) {
		hidden = true;
		for (int i = 0; i < 4; ++i)
			heart_beats[i]->stop();
	}
	
	updateSpeed();
	animation->update();
	updateHeartAnimation();
	
	row = (getShape()->position.x(1) - 8)/32;
	col = getShape()->position.x(0)/32;
	if (row == map->end_row && col == map->end_col) {
		hidden = true;
		for (int i = 0; i < 4; ++i)
			heart_beats[i]->stop();
	}
	
	getShape()->position += speed_dir*dt;
	resolveCollision();
	
	updateVision(dt);
}

void Player::render() {
	SDL_Rect tmp;
	tmp.x = floor(getShape()->position.x(0)) - sprite_vision->rectW()/2;
	tmp.y = floor(getShape()->position.x(1)) - sprite_vision->rectH()/2;
	tmp.w = sprite_vision->rectW();
	tmp.h = sprite_vision->rectH();
	SDLBase::drawMask(tmp, 0);
	sprite_vision->render(getShape()->position.x(0), getShape()->position.x(1), true);
	
	if (!sprite || hidden)
		return;
	
	if (!speed_dir.length())
		sprite->render(getShape()->position.x(0), getShape()->position.x(1), true);
	else
		animation->render(getShape()->position.x(0), getShape()->position.x(1), true);
	animation_heart->render(getShape()->position.x(0), getShape()->position.x(1), true);
}

void Player::updateSpeed() {
	speed_dir.annul();
	
	if (hidden)
		return;
	
	if (InputManager::instance()->keyPressed(SDLK_UP) || InputManager::instance()->keyPressed(SDLK_w))
		speed_dir.add(1, -1);
	if (InputManager::instance()->keyPressed(SDLK_DOWN) || InputManager::instance()->keyPressed(SDLK_s))
		speed_dir.add(1, 1);
	if (InputManager::instance()->keyPressed(SDLK_LEFT) || InputManager::instance()->keyPressed(SDLK_a))
		speed_dir.add(0, -1);
	if (InputManager::instance()->keyPressed(SDLK_RIGHT) || InputManager::instance()->keyPressed(SDLK_d))
		speed_dir.add(0, 1);
	
	if (!speed_dir.length())
		return;
	
	speed_dir = speed_dir.unitvec()*SPEED_ABS;
	Scalar angle_tmp = r2vec(0, -1)%speed_dir;
	if (speed_dir.x(0) > 0) {
		if (speed_dir.x(1) > 0)
			angle_tmp = 225;
		else if (speed_dir.x(1) < 0)
			angle_tmp = 315;
		else
			angle_tmp = 270;
	}
	sprite->rotozoom(angle_tmp);
	animation_heart->rotozoom(angle_tmp);
	selectAnimationRow(angle_tmp);
}

bool Player::cantGo(int i, int j, Scalar dt) const {
	int x = i*32 + 16;
	int y = j*32 + 24;
	Rectangle new_rect = *((Rectangle*)getShape());
	new_rect.position += speed_dir*dt;
	return Rectangle(r2vec(x, y), 32, 32).collides(new_rect);
}

void Player::updateVision(Scalar dt) {
	static Scalar t = 0;
	
	Scalar frequency = 4*Scalar(100)/hp;
	if (frequency > MAX_FREQUENCY)
		frequency = MAX_FREQUENCY;
	Scalar sqr_sin = sin(frequency*t);
	Scalar vision_scale = 0.5*(1 + hp/100) - 0.05*sqr_sin*sqr_sin;
	
	if (vision_scale > 1)
		vision_scale = 1;
	else if (vision_scale < 0.5)
		vision_scale = 0.5;
	
	sprite_vision->rotozoom(0, vision_scale, vision_scale);
	
	t += dt;
}

void Player::selectAnimationRow(Scalar angle) {
	switch (int(angle)/45) {
	case 0:
		animation->line = 0;
		break;
		
	case 1:
		animation->line = 7;
		break;
		
	case 2:
		animation->line = 6;
		break;
		
	case 3:
		animation->line = 5;
		break;
		
	case 4:
		animation->line = 4;
		break;
		
	case 5:
		animation->line = 3;
		break;
		
	case 6:
		animation->line = 2;
		break;
		
	case 7:
		animation->line = 1;
		break;
		
	default:
		break;
	}
}

void Player::updateHeartAnimation() {
	static int sound_i = -1;
	int frequency = int(4*Scalar(100)/hp);
	if (frequency > MAX_FREQUENCY)
		frequency = MAX_FREQUENCY;
	if (frequency/4 != sound_i) {
		sound_i = frequency/4;
		if (sound_i - 1 >= 0)
			heart_beats[sound_i - 1]->stop();
		heart_beats[sound_i]->play();
	}
	animation_heart->setFPS(frequency);
	animation_heart->update();
}

void Player::resolveCollision() {
	bool up = false;
	bool down = false;
	bool left = false;
	bool right = false;
	int i, j;
	
	i = row - 1;
	for (j = col - 1; j <= col + 1; ++j) {
		if (i >= 0 && i < 22 && j >= 0 && j < 40 && map->matrix[i][j])
			up = true;
	}
	i = row + 1;
	for (j = col - 1; j <= col + 1; ++j) {
		if (i >= 0 && i < 22 && j >= 0 && j < 40 && map->matrix[i][j])
			down = true;
	}
	j = col - 1;
	for (i = row - 1; i <= row + 1; ++i) {
		if (i >= 0 && i < 22 && j >= 0 && j < 40 && map->matrix[i][j])
			left = true;
	}
	j = col + 1;
	for (i = row - 1; i <= row + 1; ++i) {
		if (i >= 0 && i < 22 && j >= 0 && j < 40 && map->matrix[i][j])
			right = true;
	}
	
	if (up)
		getShape()->position.add(1, row*32 + 8 - (getShape()->position.x(1) - ((Rectangle*)getShape())->getHeight()/2));
	if (down)
		getShape()->position.add(1, (row + 1)*32 + 8 - (getShape()->position.x(1) + ((Rectangle*)getShape())->getHeight()/2));
	if (left)
		getShape()->position.add(0, col*32 - (getShape()->position.x(0) - ((Rectangle*)getShape())->getWidth()/2));
	if (right)
		getShape()->position.add(0, (col + 1)*32 - (getShape()->position.x(0) + ((Rectangle*)getShape())->getWidth()/2));
}
