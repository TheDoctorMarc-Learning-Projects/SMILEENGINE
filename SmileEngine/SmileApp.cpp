#include "SmileApp.h"

class SmileSetup;
class SmileModule;
class SmileWindow;
class SmileInput;
class SmileScene;
class SmileRenderer3D;
class SmileCamera3D;
class SmileGui;
class SmileUtiliesModule;
class SmileFBX;
class SmileGameObjectManager;
class SmileSpatialTree; 
class SmileResourceManager; 

SmileApp::SmileApp()
{
	window = DBG_NEW SmileWindow(this);
	input = DBG_NEW SmileInput(this);
	scene_intro = DBG_NEW SmileScene(this);
	renderer3D = DBG_NEW SmileRenderer3D(this);
	gui = DBG_NEW SmileGui(this);
	utilities = DBG_NEW SmileUtilitiesModule(this); 
	fbx = DBG_NEW SmileFBX(this);
	object_manager = DBG_NEW SmileGameObjectManager(this);
	spatial_tree = DBG_NEW SmileSpatialTree(this); 
	resources = DBG_NEW SmileResourceManager(this); 

	// Test 
	AddModule(utilities);

	// Main SmileModules
	AddModule(window);
	AddModule(input);
	AddModule(fbx);
	AddModule(resources); 

	// Scenes
	AddModule(object_manager);
	AddModule(scene_intro);
	AddModule(spatial_tree); 
	AddModule(gui); 

	// Renderer last!
	AddModule(renderer3D);
}

SmileApp::~SmileApp()
{
	for (auto& item : list_Modules)
		if (item != NULL)
			RELEASE(item); 
	list_Modules.clear(); 

}

bool SmileApp::Init()
{
	bool ret = true;

	// Call Init() in all SmileModules
	auto item = list_Modules.begin();

	while (item != list_Modules.end() && ret == true)
	{
		ret = (*item)->Init();
		++item; 
	}

	// After all Init calls we call Start() in all SmileModules
	LOG("SmileApp Start --------------");
    item = list_Modules.begin();

	while (item != list_Modules.end() && ret == true)
	{
		ret = (*item)->Start();
		++item;
	}

	ms_timer.Start();
	return ret;
}

// ---------------------------------------------
void SmileApp::PrepareUpdate()
{
	dt = (float)ms_timer.Read() / 1000.0f;
	ms_timer.Start();

	////FPS_LOG
	fps_log.push_back(1 / dt);
	if (fps_log.size() > 100)
	{
		fps_log.erase(fps_log.begin());
	}
	////MS_LOG
	ms_log.push_back(dt * 1000);
	if (ms_log.size() > 100)
	{
		ms_log.erase(ms_log.begin());
	}
}

// ---------------------------------------------
void SmileApp::FinishUpdate()
{
}

// Call PreUpdate, Update and PostUpdate on all SmileModules
update_status SmileApp::Update()
{
	update_status ret = UPDATE_CONTINUE;
	PrepareUpdate();
	
	auto item = list_Modules.begin();
	while (item != list_Modules.end() && ret == UPDATE_CONTINUE)
	{
		ret = (*item)->PreUpdate(dt);
		++item;
	}

	item = list_Modules.begin();
	while (item != list_Modules.end() && ret == UPDATE_CONTINUE)
	{
		ret = (*item)->Update(dt);
		++item;
	}

	item = list_Modules.begin();
	while (item != list_Modules.end() && ret == UPDATE_CONTINUE)
	{
		ret = (*item)->PostUpdate(dt);
		++item;
	}

	FinishUpdate();
	return ret;
}

bool SmileApp::CleanUp()
{
	bool ret = true;

	auto item = list_Modules.begin();
	while (item != list_Modules.end() && ret == true)
	{
		ret = (*item)->CleanUp();
		++item;
	}

	return ret;
}

void SmileApp::AddModule(SmileModule* mod)
{
	list_Modules.push_back(mod);
}