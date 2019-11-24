#pragma once

#include "Timer.h"
#include "SmileSerialization.h" // do not include this there
#include "SmileApp.h"
#include "SmileRenderer3D.h"
#include "SmileGui.h"

namespace TimeManager
{
	namespace
	{
		struct timeData
		{
			Uint32 frameCount = 0, gameTimeSec = 0, realTimeSec = 0;
			double gameTimeScale = 3, deltaGameFrameSec = 0, deltaRealFrameSec = 0;
		};

		timeData _timeData;
		Timer realTimeClock = Timer();
		Timer gameClock = Timer(false);
		static bool isPlaying = false; 
		static bool isPaused = false;
	}

	static bool IsPlaying() { return isPlaying; };
	static bool IsPaused() { return isPaused; };

	static void PlayButton() 
	{
		if (isPlaying == false)
		{
			gameClock.Start();
			App->serialization->SaveScene();
			App->SetDtMultiplier(_timeData.gameTimeScale);
			App->renderer3D->SwitchCamera(); 
		//	App->window->SetFullscreen(SDL_WINDOW_FULLSCREEN, (SDL_bool)true);
			App->scene_intro->generalDbug = !App->scene_intro->generalDbug; 
		}
		else
		{
			gameClock.Stop();
			App->serialization->LoadScene("Library/Scenes/scene.json");
			App->SetDtMultiplier(1.F);
    	//	App->window->SetFullscreen(0, (SDL_bool)false);
			App->scene_intro->generalDbug = !App->scene_intro->generalDbug;
		}
	
		isPlaying = !isPlaying;
	}; 

	static void PauseButton()
	{

		if (IsPaused() == false)
		{
			App->SetDtMultiplier(0.F);
			gameClock.Stop();
		}
		else
		{
			App->SetDtMultiplier(1.F);
			gameClock.Start();
		}

		isPaused = !isPaused; 
	}; 
 
	static void PlayOneButton()
	{
		App->SetDtMultiplier(_timeData.gameTimeScale);
		gameClock.Start();
		isPlaying = true;
		isPaused = false; 
	}

}

