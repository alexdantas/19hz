#include "Button.hpp"

#include "Rectangle.hpp"
#include "InputManager.hpp"

#define ENTER_DELAY	200

using namespace lalge;

Audio* Button::sound_hover = 0;
Audio* Button::sound_clicked = 0;

Button::Button(Sprite* sprite) :
clip_y(0),
clicked(false),
hover(false),
just_clicked(false),
just_hit(false),
toggle(false),
was_enabled(true),
selected(false),
play_sounds(true)
{
	subject.init(LASTEVENT);
	setShape(new Rectangle());
	enable(true);
	
	// width and height by sprite
	this->sprite = sprite;
	((Rectangle*)getShape())->setWidth(sprite->srcW());
	((Rectangle*)getShape())->setHeight(sprite->srcH()/4);
	
	InputManager::instance()->connect(InputManager::MOUSEDOWN_LEFT, this, &Button::handleMouseDownLeft);
	InputManager::instance()->connect(InputManager::MOUSEUP_LEFT, this, &Button::handleMouseUpLeft);
	InputManager::instance()->connect(InputManager::KEYDOWN, this, &Button::handleKeyDown);
	
	timer.connect(Timer::DONE, this, &Button::handleTimerDone);
}

Button::~Button() {
	InputManager::instance()->disconnect(this);
	timer.disconnect(this);
}

void Button::update() {
	if (!enabled)
		return;
	
	timer.update();
	
	// update
	if (clicked) {
		clicked = false;
		clip_y = 2*sprite->srcH()/4;
		hover = false;
	}
	else if (!getShape()->mouseInside()) {
		clip_y = 0;
		hover = false;
		just_hit = false;
	}
	else if (!InputManager::instance()->mousePressed(SDL_BUTTON_LEFT)) {
		clip_y = sprite->srcH()/4;
		
		// play sound
		if ((sound_hover) && (!hover)) {
			if (!just_clicked) {
				if (play_sounds)
					sound_hover->play(1);
			}
			else
				just_clicked = false;
		}
		
		hover = true;
		just_hit = false;
	}
	else if (getShape()->mouseDownInside()) {
		clip_y = 2*sprite->srcH()/4;
		hover = false;
	}
	else {
		clip_y = 0;
		hover = false;
		just_hit = false;
	}
	
	// if selected and mouse not inside, clip hover
	if (selected) {
		if (timer.time()) {
			toggle = (!toggle);
			if (toggle)
				clip_y = sprite->srcH()/4;
			else
				clip_y = 2*sprite->srcH()/4;
		}
		else if (!getShape()->mouseInside())
			clip_y = sprite->srcH()/4;
	}
	
	sprite->clip(0, clip_y, sprite->srcW(), sprite->srcH()/4);
}

void Button::enable(bool enable) {
	enabled = enable;
	if (!enable)
		sprite->clip(0, 3*sprite->srcH()/4, sprite->srcW(), sprite->srcH()/4);
}

void Button::handleMouseDownLeft(const observer::Event& event, bool& stop) {
	if (!(getShape()->mouseDownInside()))
		return;
	
	was_enabled = true;
	
	if (!enabled) {
		was_enabled = false;
		return;
	}
	
	clicked = true;
	
	// play sound
	if (sound_clicked) {
		if (play_sounds)
			sound_clicked->play(1);
		just_hit = true;
	}
}

void Button::handleMouseUpLeft(const observer::Event& event, bool& stop) {
	if ((getShape()->mouseDownInside()) && (getShape()->mouseInside()) && (enabled) && (was_enabled)) {
		clicked = true;
		just_clicked = true;
		
		// play sound
		if ((sound_clicked) && (!just_hit) && (play_sounds))
			sound_clicked->play(1);
		
		subject.broadcast(observer::Event(CLICKED));
	}
}

void Button::handleKeyDown(const observer::Event& event, bool& stop) {
	if ((inputmanager_event.key.keysym.sym == SDLK_RETURN) ||
		(inputmanager_event.key.keysym.sym == SDLK_KP_ENTER)) {
		if ((selected) && (enabled)) {
			timer.start(ENTER_DELAY);
			
			// play sound
			if ((sound_clicked) && (play_sounds))
				sound_clicked->play(1);
			
		}
	}
}

void Button::handleTimerDone(const observer::Event& event, bool& stop) {
	subject.broadcast(observer::Event(CLICKED));
}
