#pragma once

#include "Timer.h"
#include "SmileSerialization.h" // do not include this there
#include "SmileApp.h"

namespace TimeManager
{
	namespace
	{
		struct timeData
		{
			Uint32 frameCount = 0, gameTimeSec = 0, realTimeSec = 0;
			double gameTimeScale = 1.3, deltaGameFrameSec = 0, deltaRealFrameSec = 0;
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
		}
		else
		{
			gameClock.Stop();
			App->serialization->LoadScene("Library/Scenes/scene.json");
		}
	
	}; 

	void PauseButton()
	{
		gameClock.Stop(); 
	}; 

	void PlayOne() {}; 

}

