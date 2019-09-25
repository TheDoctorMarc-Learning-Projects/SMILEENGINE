#pragma once

#include "SmileSetup.h"
#include "Timer.h"
#include "SmileModule.h"
#include "SmileWindow.h"
#include "SmileInput.h"
#include "SmileAudio.h"
#include "SmileScene.h"
#include "SmileRenderer3D.h"
#include "SmileCamera3D.h"
#include "SmilePlayer.h"
#include "SmileGui.h"
#include "SmileUtilitiesModule.h"

#include <list>

class SmileApp
{
public:
	SmileWindow* window;
	SmileInput* input;
	SmileAudio* audio;
	SmileScene* scene_intro;
	SmileRenderer3D* renderer3D;
	SmileCamera3D* camera;
	SmilePlayer* player;
	SmileGui* gui; 
	SmileUtilitiesModule* test; 

private:

	Timer	ms_timer;
	float	dt;
	std::list<SmileModule*> list_Modules;

public:

	SmileApp();
	~SmileApp();

	bool Init();
	update_status Update();
	bool CleanUp();

	float GetDT() const { return dt; }; 

private:

	void AddModule(SmileModule* mod);
	void PrepareUpdate();
	void FinishUpdate();
};

