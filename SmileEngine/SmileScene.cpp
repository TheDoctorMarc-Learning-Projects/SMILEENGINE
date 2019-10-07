#define PAR_SHAPES_IMPLEMENTATION
#include "parshapes/par_shapes.h"

#include "SmileSetup.h"
#include "SmileApp.h"
#include "SmileScene.h"
/*#include <gl/GL.h>
//#include <gl/GLU.h>*/

#include "Glew/include/GL/glew.h"

#include "MathGeoLib/include/MathGeoLib.h"


#ifdef NDEBUG //no debug
#pragma comment (lib, "MathGeoLib/libx86/ReleaseLib/MathGeoLib.lib") 
#else
#pragma comment (lib, "MathGeoLib/libx86/DebugLib/MathGeoLib.lib") 
#endif


SmileScene::SmileScene(SmileApp* app, bool start_enabled) : SmileModule(app, start_enabled)
{
}

SmileScene::~SmileScene()
{}

// Load assets
bool SmileScene::Start()
{ 
	//testCube = par_shapes_create_cube(); 

	// Vertex buffer
	App->fbx->ReadFBXData("..//Assets/warrior.FBX");

	return true;
}


// Load assets
bool SmileScene::CleanUp()
{
	//par_shapes_free_mesh(testCube); 
	meshes.clear(); 

	return true;
}

// Update
update_status SmileScene::Update(float dt)
{
    
	DrawGrid();
 
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	// Test: drawing a par_shapes cube 
	/*if(App->input->GetKey(SDL_SCANCODE_P)==KEY_DOWN)
		App->fbx->ReadFBXData("..//Assets/warrior.FBX");*/
 
	return UPDATE_CONTINUE;
}

void SmileScene::DrawGrid()
{
	
	// TODO
}

void SmileScene::DrawMeshes()
{
	for (auto& mesh : meshes)
		App->fbx->DrawMesh(mesh); 
}

