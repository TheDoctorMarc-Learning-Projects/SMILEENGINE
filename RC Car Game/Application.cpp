#include "Application.h"
#include "Globals.h"

Application::Application()
{
	window = DBG_NEW ModuleWindow(this);
	input = DBG_NEW ModuleInput(this);
	audio = DBG_NEW ModuleAudio(this, true);
	scene_intro = DBG_NEW ModuleSceneIntro(this);
	renderer3D = DBG_NEW ModuleRenderer3D(this);
	camera = DBG_NEW ModuleCamera3D(this);
	physics = DBG_NEW ModulePhysics3D(this);
	player = DBG_NEW ModulePlayer(this);

	// The order of calls is very important!
	// Modules will Init() Start() and Update in this order
	// They will CleanUp() in reverse order

	// Main Modules
	AddModule(window);
	AddModule(input);
	AddModule(audio);
	AddModule(physics);
	
	// Scenes
	AddModule(scene_intro);
	AddModule(player);
	
	AddModule(camera);

	// Renderer last!
	AddModule(renderer3D);
}

Application::~Application()
{
	p2List_item<Module*>* item = list_modules.end;

	while(item != NULL)
	{
		delete item->data;
		item = item->prev;
	}
}

bool Application::Init()
{
	bool ret = true;

	// Call Init() in all modules
	p2List_item<Module*>* item = list_modules.start;

	while(item != NULL && ret == true)
	{
		ret = item->data->Init();
		item = item->next;
	}

	// After all Init calls we call Start() in all modules
	LOG("Application Start --------------");
	item = list_modules.start;

	while(item != NULL && ret == true)
	{
		ret = item->data->Start();
		item = item->next;
	}
	
	ms_timer.Start();
	return ret;
}

// ---------------------------------------------
void Application::PrepareUpdate()
{
	dt = (float)ms_timer.Read() / 1000.0f;
	ms_timer.Start();
}

// ---------------------------------------------
void Application::FinishUpdate()
{
}

// Call PreUpdate, Update and PostUpdate on all modules
update_status Application::Update()
{
	update_status ret = UPDATE_CONTINUE;
	PrepareUpdate();
	
	p2List_item<Module*>* item = list_modules.start;
	
	while(item != NULL && ret == UPDATE_CONTINUE)
	{
		ret = item->data->PreUpdate(dt);
		item = item->next;
	}

	item = list_modules.start;

	while(item != NULL && ret == UPDATE_CONTINUE)
	{
		ret = item->data->Update(dt);
		item = item->next;
	}

	item = list_modules.start;

	while(item != NULL && ret == UPDATE_CONTINUE)
	{
		ret = item->data->PostUpdate(dt);
		item = item->next;
	}

	FinishUpdate();
	return ret;
}

bool Application::CleanUp()
{
	bool ret = true;
	p2List_item<Module*>* item = list_modules.end;

	while(item != NULL && ret == true)
	{
		ret = item->data->CleanUp();
		item = item->prev;
	}
	return ret;
}

void Application::AddModule(Module* mod)
{
	list_modules.add(mod);
}