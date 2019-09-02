#ifndef __j1AUDIO_H__
#define __j1AUDIO_H__

#include "SmileModule.h"
#include "SDL_mixer/include/SDL_mixer.h"
#include <list>
#include <string>
#include <map>
#include "Timer.h"

#define DEFAULT_MUSIC_FADE_TIME 2.0f

struct _Mix_Music;
struct Mix_Chunk;

struct Mix_Chunk_Avanced
{
	Mix_Chunk* chunk;
	int actualChannel = -1;
};

class SmileAudio : public SmileModule
{
public:

	SmileAudio(SmileApp* app, bool start_enabled = true);
	~SmileAudio();

	// Called before render is available
	bool Init();

	bool Start();

	update_status Update(float dt);

	// Called before quitting
	bool CleanUp();

	// Play a music file
	bool PlayMusic(const char* path, float fade_time = DEFAULT_MUSIC_FADE_TIME);

	// Stop the current muisc
	void PauseMusic()
	{
		Mix_PauseMusic();
	}

	// Resume the current muisc
	void ResumeMusic()
	{
		Mix_ResumeMusic();
	}

	// Load a WAV in memory
	unsigned int LoadFx(const char* path, std::string wantedName);

	// Play a previously loaded WAV
	bool PlayFx(std::string name, int repeat = 0, bool prioritary = false, float volume = (float)INT_MAX, float musReduc = (float)INT_MAX, float fxsReduc = (float)INT_MAX);

	//Set specific FX volume
	bool SetSpecificFxVolume(std::string name, float volume);

	// Set music volume
	void SetVolume(float volume);

	// Set FX volume
	void SetFxVolume(float volume);

	// Set both
	void ResetMusicAndFxVolumes();

	// Stop all FXs
	void StopAllFxs();

	// Stop specific FX
	void StopSpecificFx(std::string name);

	// Pause specific FX
	void PauseSpecificFx(std::string name);

	// Resume specific FX
	void ResumeSpecificFx(std::string name);

	// Gather the status of a FX
	bool isPlayingFx(std::string name);
	bool isPausedFx(std::string name);

	// check if and fx exitst in the map
	bool existsFx(std::string name) const
	{
		auto item = fxMap.find(name);
		if (item != fxMap.end())
			return true;
		return false;
	}

private:

	_Mix_Music*			music = nullptr;
	std::string prioritaryChunk = "empty";
	bool toUpdate = false;
	float final_volume = MIX_MAX_VOLUME;
	float final_fx_volume = MIX_MAX_VOLUME;
	float last_volume = 0.0f;
	float last_fx_volume = 0.0f;
	float previous_volume = 0.0f;
	float previous_fx_volume = 0.0f;

	Timer musTimer;

public:
	std::map<std::string, Mix_Chunk_Avanced> fxMap;

};

#endif // __j1AUDIO_H__   