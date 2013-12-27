#include "Interaction.hpp"

#include "common.hpp"

Interaction::Interaction(GameObject* go1, GameObject* go2, callback handler, bool twice) :
go1(go1), go2(go2), handler(handler), enabled(true), twice(twice) {}

void Interaction::interact() {
	if (enabled) {
		COMMON_CALLBACK(*go1, handler)(go2, enabled);
		if (twice)
			COMMON_CALLBACK(*go2, handler)(go1, enabled);
	}
}
