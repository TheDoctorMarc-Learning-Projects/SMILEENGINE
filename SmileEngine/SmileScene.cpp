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
	

	return true;
}


// Load assets
bool SmileScene::CleanUp()
{
	//par_shapes_free_mesh(testCube); 
	for (auto& fbx : fbxs)
	{
		for (auto& mesh : fbx.meshes)
			App->fbx->FreeMeshBuffers(mesh); 
		fbx.meshes.clear();
	}
	fbxs.clear(); 

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

void SmileScene::DrawMeshes()
{
	for(auto& fbx : fbxs)
		for (auto& mesh : fbx.meshes)
			App->fbx->DrawMesh(mesh);

}

