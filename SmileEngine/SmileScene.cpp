#include "SmileSetup.h"
#include "SmileApp.h"
#include "SmilePhysics3D.h"
#include "Primitive.h"
#include "PhysBody3D.h"
#include <math.h>
#include <gl/GL.h>
#include <gl/GLU.h>


SmileSmileScene::SmileSmileScene(SmileApp* app, bool start_enabled) : SmileModule(app, start_enabled)
{
}

SmileSmileScene::~SmileSmileScene()
{}

// Load assets
bool SmileSmileScene::Start()
{ 
	bool my_tool_active = true; 
	float color[4] = { 255.f, 255.f, 255.f, 255.f }; 

	return true;
}


// Load assets
bool SmileSmileScene::CleanUp()
{
	
	return true;
}

// Update
update_status SmileSmileScene::Update(float dt)
{


	return UPDATE_CONTINUE;
}
