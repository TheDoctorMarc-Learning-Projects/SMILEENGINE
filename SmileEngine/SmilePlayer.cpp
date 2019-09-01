#include "SmileSetup.h"
#include "SmileApp.h"
#include "SmileSmilePlayer.h"
#include <gl/GL.h>
#include <gl/GLU.h>

SmileSmilePlayer::SmileSmilePlayer(SmileApp* app, bool start_enabled) : SmileModule(app, start_enabled)
{
}

SmileSmilePlayer::~SmileSmilePlayer()
{}

// Load assets
bool SmileSmilePlayer::Start()
{
	return true;
}

// Unload assets
bool SmileSmilePlayer::CleanUp()
{
	LOG("Unloading player");

	return true;
}

// Update: draw background
update_status SmileSmilePlayer::Update(float dt)
{
	return UPDATE_CONTINUE;
}
