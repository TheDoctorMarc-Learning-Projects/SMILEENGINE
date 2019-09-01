#include "SmileSetup.h"
#include "SmileApp.h"
#include "ModulePhysics3D.h"
#include "Primitive.h"
#include "PhysBody3D.h"
#include <math.h>
#include <gl/GL.h>
#include <gl/GLU.h>


ModuleSceneIntro::ModuleSceneIntro(SmileApp* app, bool start_enabled) : Module(app, start_enabled)
{
}

ModuleSceneIntro::~ModuleSceneIntro()
{}

// Load assets
bool ModuleSceneIntro::Start()
{ 
	bool my_tool_active = true; 
	float color[4] = { 255.f, 255.f, 255.f, 255.f }; 

	return true;
}


// Load assets
bool ModuleSceneIntro::CleanUp()
{
	
	return true;
}

// Update
update_status ModuleSceneIntro::Update(float dt)
{


	return UPDATE_CONTINUE;
}
