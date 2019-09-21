#include "SmileSetup.h"
#include "SmileApp.h"
#include <math.h>
#include <gl/GL.h>
#include <gl/GLU.h>

 

SmileScene::SmileScene(SmileApp* app, bool start_enabled) : SmileModule(app, start_enabled)
{
}

SmileScene::~SmileScene()
{}

// Load assets
bool SmileScene::Start()
{ 
	bool my_tool_active = true; 
	float color[4] = { 255.f, 255.f, 255.f, 255.f }; 

	return true;
}


// Load assets
bool SmileScene::CleanUp()
{
	
	return true;
}

// Update
update_status SmileScene::Update(float dt)
{

	// grid ground
	Plane p(0, 1, 0, 0);
	p.axis = true;
	p.Render();

	return UPDATE_CONTINUE;
}
