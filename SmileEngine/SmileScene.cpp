#define PAR_SHAPES_IMPLEMENTATION
#include "parshapes/par_shapes.h"

#include "SmileSetup.h"
#include "SmileApp.h"
#include "SmileScene.h"
/*#include <gl/GL.h>
//#include <gl/GLU.h>*/

#include "Glew/include/GL/glew.h"

SmileScene::SmileScene(SmileApp* app, bool start_enabled) : SmileModule(app, start_enabled)
{
}

SmileScene::~SmileScene()
{}

// Load assets
bool SmileScene::Start()
{ 
	App->fbx->ReadFBXData("..//Assets/BakerHouse.fbx"); 

	return true;
}


// Load assets
bool SmileScene::CleanUp()
{
	for (auto& gameObject : objects)
	{
		gameObject->CleanUp(); 
		RELEASE(gameObject); 
	}
	objects.clear();

	selected_mesh = nullptr; 

	return true;
}

// Update
update_status SmileScene::Update(float dt)
{
    
	DrawGrid();
 
	return UPDATE_CONTINUE;
}

void SmileScene::DrawGrid()
{
	
	// TODO
}

void SmileScene::UpdateGameObjects()
{
	for (auto& gameObject : objects)
		gameObject->Update(); 

}

