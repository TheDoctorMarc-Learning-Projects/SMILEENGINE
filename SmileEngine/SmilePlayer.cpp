#include "SmileSetup.h"
#include "SmileApp.h"
#include "SmilePlayer.h"
#include <gl/GL.h>
#include <gl/GLU.h>

SmilePlayer::SmilePlayer(SmileApp* app, bool start_enabled) : SmileModule(app, start_enabled)
{
}

SmilePlayer::~SmilePlayer()
{}

// Load assets
bool SmilePlayer::Start()
{
	return true;
}

// Unload assets
bool SmilePlayer::CleanUp()
{
	LOG("Unloading player");

	return true;
}

// Update: draw background
update_status SmilePlayer::Update(float dt)
{
	return UPDATE_CONTINUE;
}
