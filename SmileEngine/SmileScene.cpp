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

	ComponentTransform* rootTransf = dynamic_cast<ComponentTransform*>(std::get<Component*>(rootObj->GetComponent(TRANSFORM))); 
	rootTransf->CleanUp();
	RELEASE(rootTransf); 

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

	glLineWidth(1.0f);
	glBegin(GL_LINES);
	glColor4fv((float*)&float4(1, 1, 1, 1));

	static int sunX = 2, sunZ = 150, sunW = 2, sunQ = 150, separator = 2; 
	for (int i = -75; i <= 75; i++)
	{

		sunX = separator * i;


		glVertex3f(sunX, 0.0f, -sunZ);
		glVertex3f(sunX, 0, sunZ);


		sunQ = separator * i;


		glVertex3f(-sunW, 0.0f, sunQ);
		glVertex3f(sunW, 0, sunQ);
	}
	glEnd();
}

