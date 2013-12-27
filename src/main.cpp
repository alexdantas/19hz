
#include "common.hpp"

#include "StateManager.hpp"

using namespace common;

int main(int argc, char** argv) {
	MainArgs::init(argc, argv);
	
	try {
		StateManager sm;
		sm.run();
	} catch (Quit&) {
		// just quit
	} catch (mexception& e) {
		e.logerr();	// log the error
	}
	
	return 0;
}
