#include "SmileSetup.h"
#include "SmileApp.h"
#include "ModulePlayer.h"
#include <gl/GL.h>
#include <gl/GLU.h>

ModulePlayer::ModulePlayer(SmileApp* app, bool start_enabled) : Module(app, start_enabled)
{
}

ModulePlayer::~ModulePlayer()
{}

// Load assets
bool ModulePlayer::Start()
{
	return true;
}

// Unload assets
bool ModulePlayer::CleanUp()
{
	LOG("Unloading player");

	return true;
}

// Update: draw background
update_status ModulePlayer::Update(float dt)
{
	return UPDATE_CONTINUE;
}
