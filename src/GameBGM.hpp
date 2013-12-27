
#ifndef GAMEBGM_HPP
#define GAMEBGM_HPP

#include "Audio.hpp"

class GameBGM {
private:
	static bool playing;
	static Audio* bgm;
public:
	static void init();
	static void close();
	
	static void play();
	static void stop();
};

#endif
