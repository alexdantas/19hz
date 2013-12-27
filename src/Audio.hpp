#ifndef AUDIO_HPP
#define AUDIO_HPP

#include <string>

#include "SDL_mixer.h"

class Audio {
private:
	static float sound_volume;
	static float music_volume;
	static bool sound_mute;
	static bool music_mute;
	
	bool paused;
	
	Mix_Chunk* sound;
	Mix_Music* music;
	int channel;
public:
	Audio(const std::string& filename);
	~Audio();
	
	void play(int n = 0);
	void stop(int fade = 0);
	void pause();
	void resume();
	
	static float soundVolume(float volume = -1);
	static float musicVolume(float volume = -1);
	static void soundMute(bool flag);
	static void musicMute(bool flag);
	static bool soundMuted();
	static bool musicMuted();
};

#endif
