#include "SmileApp.h"

SmilePhysics3D::SmilePhysics3D(SmileApp* app, bool start_enabled) : SmileModule(app, start_enabled)
{
}

// Destructor
SmilePhysics3D::~SmilePhysics3D()
{ 
}

// Render not available yet----------------------------------
bool SmilePhysics3D::Init()
{
	return true;
}

// ---------------------------------------------------------
bool SmilePhysics3D::Start()
{

	return true;
}

// ---------------------------------------------------------
update_status SmilePhysics3D::PreUpdate(float dt)
{

	return UPDATE_CONTINUE;
}

// ---------------------------------------------------------
update_status SmilePhysics3D::Update(float dt)
{
	return UPDATE_CONTINUE;
}

// ---------------------------------------------------------
update_status SmilePhysics3D::PostUpdate(float dt)
{
	return UPDATE_CONTINUE;
}

// Called before quitting
bool SmilePhysics3D::CleanUp()
{
	return true;
}
