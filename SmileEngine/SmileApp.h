#pragma once

#include "SmileSetup.h"
#include "Timer.h"
#include <vector>
#include "SmileModule.h"
#include "SmileWindow.h"
#include "SmileInput.h"
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
	SmileScene* scene_intro;
	SmileRenderer3D* renderer3D;
	SmileCamera3D* camera;
	SmilePlayer* player;
	SmileGui* gui; 
	SmileUtilitiesModule* utilities;

private:

	Timer	ms_timer;
	
	float	dt;
	bool    terminated;
	std::list<SmileModule*> list_Modules;

public:
	std::vector<float> fps_log;
	std::vector<float> ms_log;
	
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

extern SmileApp *App;

