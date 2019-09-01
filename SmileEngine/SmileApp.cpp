#include "SmileApp.h"
#include "SmileSetup.h"

SmileApp::SmileApp()
{
	window = DBG_NEW SmileWindow(this);
	input = DBG_NEW SmileInput(this);
	audio = DBG_NEW SmileAudio(this, true);
	scene_intro = DBG_NEW SmileScene(this);
	renderer3D = DBG_NEW SmileRenderer3D(this);
	camera = DBG_NEW SmileCamera3D(this);
	physics = DBG_NEW SmilePhysics3D(this);
	player = DBG_NEW SmilePlayer(this);
	gui = DBG_NEW SmileGui(this);


	// The order of calls is very important!
	// SmileModules will Init() Start() and Update in this order
	// They will CleanUp() in reverse order

	// Main SmileModules
	AddSmileModule(window);
	AddSmileModule(input);
	AddSmileModule(audio);
	AddSmileModule(physics);
	
	// Scenes
	AddSmileModule(scene_intro);
	AddSmileModule(player);
	AddSmileModule(camera);
	AddSmileModule(gui); 

	// Renderer last!
	AddSmileModule(renderer3D);
}

SmileApp::~SmileApp()
{
	p2List_item<SmileModule*>* item = list_SmileModules.end;

	while(item != NULL)
	{
		delete item->data;
		item = item->prev;
	}
}

bool SmileApp::Init()
{
	bool ret = true;

	// Call Init() in all SmileModules
	p2List_item<SmileModule*>* item = list_SmileModules.start;

	while(item != NULL && ret == true)
	{
		ret = item->data->Init();
		item = item->next;
	}

	// After all Init calls we call Start() in all SmileModules
	LOG("SmileApp Start --------------");
	item = list_SmileModules.start;

	while(item != NULL && ret == true)
	{
		ret = item->data->Start();
		item = item->next;
	}
	
	ms_timer.Start();
	return ret;
}

// ---------------------------------------------
void SmileApp::PrepareUpdate()
{
	dt = (float)ms_timer.Read() / 1000.0f;
	ms_timer.Start();
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
	
	p2List_item<SmileModule*>* item = list_SmileModules.start;
	
	while(item != NULL && ret == UPDATE_CONTINUE)
	{
		ret = item->data->PreUpdate(dt);
		item = item->next;
	}

	item = list_SmileModules.start;

	while(item != NULL && ret == UPDATE_CONTINUE)
	{
		ret = item->data->Update(dt);
		item = item->next;
	}

	item = list_SmileModules.start;

	while(item != NULL && ret == UPDATE_CONTINUE)
	{
		ret = item->data->PostUpdate(dt);
		item = item->next;
	}

	FinishUpdate();
	return ret;
}

bool SmileApp::CleanUp()
{
	bool ret = true;
	p2List_item<SmileModule*>* item = list_SmileModules.end;

	while(item != NULL && ret == true)
	{
		ret = item->data->CleanUp();
		item = item->prev;
	}
	return ret;
}

void SmileApp::AddSmileModule(SmileModule* mod)
{
	list_SmileModules.add(mod);
}