#pragma once

#include "Timer.h"
#include <vector>
#include <list>

#include "SmileSetup.h"
#include "SmileModule.h"
#include "SmileWindow.h"
#include "SmileInput.h"
#include "SmileScene.h"
#include "SmileRenderer3D.h"
#include "SmileGui.h"
#include "SmileUtilitiesModule.h"
#include "SmileFBX.h"
#include "SmileGameObjectManager.h" 
#include "SmileSpatialTree.h"

class SmileApp
{
public:
	SmileWindow* window;
	SmileInput* input;
	SmileScene* scene_intro;
	SmileRenderer3D* renderer3D;
	SmileGui* gui; 
	SmileUtilitiesModule* utilities;
	SmileFBX* fbx;
	SmileGameObjectManager* object_manager;
	SmileSpatialTree* spatial_tree; 

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

