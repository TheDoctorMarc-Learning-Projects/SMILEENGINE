#pragma once

#include "Timer.h"
#include "SmileSerialization.h" // do not include this there
#include "SmileApp.h"
#include "SmileRenderer3D.h"

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
	}

	bool IsPlaying() { return gameClock.IsRunning(); };

	void PlayButton() 
	{
		if (IsPlaying() == false)
		{
			gameClock.Start();
			App->serialization->SaveScene();
			App->SetDtMultiplier(_timeData.gameTimeScale);
			App->renderer3D->SwitchCamera(); 
			App->window->SetFullscreen(SDL_WINDOW_FULLSCREEN_DESKTOP, (SDL_bool)true);
		}
		else
		{
			gameClock.Stop();
			App->renderer3D->SwitchCamera();
			App->SetDtMultiplier(1.F);
			App->serialization->LoadScene("Library/Scenes/scene.json");
			App->window->SetFullscreen(0, (SDL_bool)false);
		}
	
	}; 

	void PauseButton()
	{

		if (IsPlaying() == true)
		{
			App->SetDtMultiplier(0.F);
			gameClock.Stop();
		}
		else
		{
			App->SetDtMultiplier(1.F);
			gameClock.Start();
		}
		
	}; 

	void PlayOne() {}; 

}

