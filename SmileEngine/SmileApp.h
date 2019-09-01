#pragma once

#include "p2List.h"
#include "SmileSetup.h"
#include "Timer.h"
#include "SmileModule.h"
#include "SmileWindow.h"
#include "SmileInput.h"
#include "SmileAudio.h"
#include "SmileScene.h"
#include "SmileRenderer3D.h"
#include "SmileCamera3D.h"
#include "SmilePhysics3D.h"
#include "SmilePlayer.h"
#include "SmileGui.h"

class SmileApp
{
public:
	SmileWindow* window;
	SmileInput* input;
	SmileAudio* audio;
	SmileScene* scene_intro;
	SmileRenderer3D* renderer3D;
	SmileCamera3D* camera;
	SmilePhysics3D* physics;
	SmilePlayer* player;
	SmileGui* gui; 

private:

	Timer	ms_timer;
	float	dt;
	p2List<SmileModule*> list_SmileModules;

public:

	SmileApp();
	~SmileApp();

	bool Init();
	update_status Update();
	bool CleanUp();

	float GetDT() const { return dt; }; 

private:

	void AddSmileModule(SmileModule* mod);
	void PrepareUpdate();
	void FinishUpdate();
};