#define PAR_SHAPES_IMPLEMENTATION
#include "parshapes/par_shapes.h"

#include "SmileSetup.h"
#include "SmileApp.h"
#include "SmileScene.h"
#include "ComponentTransform.h"
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
	ComponentTransform* transf = DBG_NEW ComponentTransform(); 
	transf->SetLocalMatrix(math::float4x4::identity); 

	rootObj = DBG_NEW GameObject((Component*)transf);
	rootObj->SetName("root"); 
	//objects.push_back(rootObj); 

	App->fbx->ReadFBXData("..//Assets/BakerHouse.fbx"); 

	return true;
}

bool SmileScene::CleanUp()
{
	for (auto& gameObject : rootObj->GetChildrenRecursive())
	{
		gameObject->CleanUp(); 
		RELEASE(gameObject); 
	}
	rootObj->childObjects.clear();

	RELEASE(rootObj); 

	selectedObj = nullptr; 
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

