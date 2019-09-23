#include "SmileSetup.h"
#include "SmileApp.h"
#include "SmileScene.h"
#include <math.h>
#include <gl/GL.h>
#include <gl/GLU.h>

#include "MathGeoLib/include/MathGeoLib.h"

#ifdef NDEBUG //no debug
#pragma comment (lib, "MathGeoLib/libx86/Release/MathGeoLib.lib") 
#else
#pragma comment (lib, "MathGeoLib/libx86/Debug/MathGeoLib.lib") 
#endif


SmileScene::SmileScene(SmileApp* app, bool start_enabled) : SmileModule(app, start_enabled)
{
}

SmileScene::~SmileScene()
{}

// Load assets
bool SmileScene::Start()
{ 

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
    
	math::Sphere a = math::Sphere(math::float3(0.F, 0.F, 0.F), 1);
	math::Sphere b = math::Sphere(math::float3(0.F, 0.F, 0.F), 2);

	if (a.Intersects(b) == true)
		LOG("An intersection between two physical objects has been detected!!"); 


 
	return UPDATE_CONTINUE;
}
