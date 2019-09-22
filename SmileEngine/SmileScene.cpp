#include "SmileSetup.h"
#include "SmileApp.h"
#include "SmileScene.h"
#include <math.h>
#include <gl/GL.h>
#include <gl/GLU.h>

#include "MathGeoLib/include/MathGeoLib.h"
#include "MathGeoLib/include/Geometry/Sphere.h"


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
		int a = 0; 


 
	return UPDATE_CONTINUE;
}
