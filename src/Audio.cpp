#include "common.hpp"

#include "Audio.hpp"

#include "SDLBase.hpp"

using namespace common;

using std::string;

float Audio::sound_volume = 1;
float Audio::music_volume = 1;
bool Audio::sound_mute = false;
bool Audio::music_mute = false;

Audio::Audio(const std::string& filename) : sound(NULL), music(NULL), channel(0)
{
	if (!SDLBase::hasAudio())
		return;
	
	if( filename[ filename.size() - 1 ] == 'v' )
	{
		sound = Mix_LoadWAV( RootPath::get( filename ).c_str() );
		if( !sound )
			throw( mexception( "Mix_LoadWAV error" ) );
	}
	else
	{
		music = Mix_LoadMUS( RootPath::get( filename ).c_str() );
		if( !music )
			throw( mexception( "Mix_LoadMUS error" ) );
	}
}

Audio::~Audio()
{
	if (!SDLBase::hasAudio())
		return;
	
	if( sound )
		Mix_FreeChunk( sound );
	else
		Mix_FreeMusic( music );
}

void Audio::play(int n)
{
	if (!SDLBase::hasAudio())
		return;
	
	if( sound )
		channel = Mix_PlayChannel( -1, sound, n - 1 );
	else
		Mix_PlayMusic( music, n - 1 );
}

void Audio::stop(int fade)
{
	if (!SDLBase::hasAudio())
		return;
	
	if( sound )
		Mix_HaltChannel( channel );
	else
		Mix_FadeOutMusic( fade );
}

void Audio::pause()
{
	if (!SDLBase::hasAudio())
		return;
	
	if( sound )
		Mix_Pause( channel );
	else
		Mix_PauseMusic();
}

void Audio::resume()
{
	if (!SDLBase::hasAudio())
		return;
	
	if( sound )
		Mix_Resume( channel );
	else
		Mix_ResumeMusic();
}

float Audio::soundVolume(float volume)
{
	if (!SDLBase::hasAudio())
		return 0;
	
	if( ( volume >= 0 ) && ( volume <= 1 ) )
	{
		sound_volume = volume;
		if( !sound_mute )
			Mix_Volume( -1, sound_volume * MIX_MAX_VOLUME );
	}
	
	return sound_volume;
}

float Audio::musicVolume(float volume)
{
	if (!SDLBase::hasAudio())
		return 0;
	
	if( ( volume >= 0 ) && ( volume <= 1 ) )
	{
		music_volume = volume;
		if( !music_mute )
			Mix_VolumeMusic( music_volume * MIX_MAX_VOLUME );
	}
	
	return music_volume;
}

void Audio::soundMute(bool flag)
{
	if (!SDLBase::hasAudio())
		return;
	
	if( ( !sound_mute ) && ( flag ) )
	{
		Mix_Volume( -1, 0 );
		sound_mute = true;
	}
	else if( ( sound_mute ) && ( !flag ) )
	{
		Mix_Volume( -1, sound_volume * MIX_MAX_VOLUME );
		sound_mute = false;
	}
}

void Audio::musicMute(bool flag)
{
	if (!SDLBase::hasAudio())
		return;
	
	if( ( !music_mute ) && ( flag ) )
	{
		Mix_VolumeMusic( 0 );
		music_mute = true;
	}
	else if( ( music_mute ) && ( !flag ) )
	{
		Mix_VolumeMusic( music_volume * MIX_MAX_VOLUME );
		music_mute = false;
	}
}

bool Audio::soundMuted()
{
	if (!SDLBase::hasAudio())
		return false;
	
	return sound_mute;
}

bool Audio::musicMuted()
{
	if (!SDLBase::hasAudio())
		return false;
	
	return music_mute;
}
