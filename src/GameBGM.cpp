#include "GameBGM.hpp"

bool GameBGM::playing = false;
Audio* GameBGM::bgm = 0;

void GameBGM::init() {
	if (!bgm) {
		bgm = new Audio("sfx/bgm.mp3");
		bgm->play();
		playing = true;
	}
}

void GameBGM::close() {
	if (bgm) {
		if (playing) {
			bgm->stop();
			playing = false;
		}
		delete bgm;
		bgm = 0;
	}
}

void GameBGM::play() {
	if ((!playing) && (bgm)) {
		bgm->play();
		playing = true;
	}
}

void GameBGM::stop() {
	if ((playing) && (bgm)) {
		bgm->stop();
		playing = false;
	}
}
