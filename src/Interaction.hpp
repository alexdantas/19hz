
#ifndef INTERACTION_HPP
#define INTERACTION_HPP

#include "GameObject.hpp"

class Interaction {
public:
	typedef void (GameObject::*callback)(GameObject*, bool&);
private:
	GameObject* go1;
	GameObject* go2;
public:
	callback handler;
	
	bool twice;
	bool enabled;
	
	Interaction(GameObject* go1, GameObject* go2, callback handler, bool twice = false);
	void interact();
};

#endif
