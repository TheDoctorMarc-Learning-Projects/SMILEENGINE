#include "Globals.h"
#include "Application.h"
#include "ModulePhysics3D.h"
#include "Primitive.h"
#include "PhysBody3D.h"
#include <math.h>
#include <gl/GL.h>
#include <gl/GLU.h>

ModuleSceneIntro::ModuleSceneIntro(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

ModuleSceneIntro::~ModuleSceneIntro()
{}

// Load assets
bool ModuleSceneIntro::Start()
{ 
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
