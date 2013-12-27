
#ifndef INPUTMANAGER_HPP
#define INPUTMANAGER_HPP

#include "SDL.h"

#include "observer.hpp"

#define inputmanager_event	(*(*((InputManager::Event*)&event))())

class InputManager {
SUBJECT
public:
	enum {
		// events
		QUIT = 0,
		KEYDOWN,
		KEYUP,
		MOUSEMOTION,
		MOUSEDOWN_LEFT,
		MOUSEDOWN_MIDDLE,
		MOUSEDOWN_RIGHT,
		MOUSEUP_LEFT,
		MOUSEUP_MIDDLE,
		MOUSEUP_RIGHT,
		MOUSE_WHEELUP,
		MOUSE_WHEELDOWN,
		
		// don't change this
		LASTEVENT
	};
	
	class Event : public observer::Event {
	private:
		SDL_Event* event;
	public:
		Event(int type, SDL_Event* event);
		const SDL_Event* operator()() const;
	};
private:
	static InputManager* instance_;
	
	SDL_Event event_;
	
	bool key_pressed[SDLK_LAST];
	int mouse_x;
	int mouse_y;
	bool mouse_pressed[6];
	int mousedown_x;
	int mousedown_y;
	
	InputManager();
	~InputManager();
public:
	static InputManager* instance();
	static void close();
	
	void update();
	
	bool keyPressed(int i) const;
	int mouseX() const;
	int mouseY() const;
	bool mousePressed(int i) const;
	int mouseDownX() const;
	int mouseDownY() const;
	int mouseDiffX() const;
	int mouseDiffY() const;
};

#endif
