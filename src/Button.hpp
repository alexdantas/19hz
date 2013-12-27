#ifndef BUTTON_HPP
#define BUTTON_HPP

#include "observer.hpp"

#include "GameObject.hpp"
#include "Audio.hpp"
#include "Timer.hpp"

class Button : public GameObject {
SUBJECT
public:
	enum {
		CLICKED = 0,
		
		// don't change this
		LASTEVENT
	};
private:
	int clip_y;
	bool clicked;
	bool enabled;
	bool hover;
	bool just_clicked;
	bool just_hit;
	Timer timer;
	bool toggle;
	bool was_enabled;
public:
	bool selected;
	bool play_sounds;
	
	static Audio* sound_hover;
	static Audio* sound_clicked;
	
	Button(Sprite* sprite);
	~Button();
	
	void update();
	
	void enable(bool enable);
private:
	void handleMouseDownLeft(const observer::Event& event, bool& stop);
	void handleMouseUpLeft(const observer::Event& event, bool& stop);
	void handleKeyDown(const observer::Event& event, bool& stop);
	void handleTimerDone(const observer::Event& event, bool& stop);
};

#endif
