#define PAR_SHAPES_IMPLEMENTATION
#include "parshapes/par_shapes.h"

#include "SmileSetup.h"
#include "SmileApp.h"
#include "SmileScene.h"
#include "ComponentTransform.h"
/*#include <gl/GL.h>
//#include <gl/GLU.h>*/

#include "Glew/include/GL/glew.h"
#include "GameObjectCamera.h"
#include "SmileGameObjectManager.h"

SmileScene::SmileScene(SmileApp* app, bool start_enabled) : SmileModule(app, start_enabled)
{
}

SmileScene::~SmileScene()
{}

// Load assets
bool SmileScene::Start()
{ 
	// Root
	rootObj = DBG_NEW GameObject(DBG_NEW ComponentTransform(), "root");

	// Default Scene
	App->fbx->ReadFBXData("Assets/Models/BakerHouse.fbx"); 

	// Debug Camera
	debugCamera = App->object_manager->CreateCamera(rootObj, vec3(0, 20, 10), vec3(0, 0, 9));
	
	// Game Camera
	renderingData data; 
	data.pFarDist = 10.f; 
	gameCamera = App->object_manager->CreateCamera(rootObj, vec3(0, 0, 5), vec3(0, 0, 0), data);

	return true;
}

bool SmileScene::CleanUp()
{
	rootObj->CleanUp(); // does recursion
	RELEASE(rootObj); 

	selectedObj = nullptr; 
	selected_mesh = nullptr; 
	debugCamera = nullptr; 
	gameCamera = nullptr; 

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
	glLineWidth(lineWidth);
	glColor3f(1, 1, 1);
	glBegin(GL_LINES);
	for (float i = 0; i <= MAXLINES; i++)
	{
		glVertex3f(i - MAXLINES * linesLength, 0, -MAXLINES * linesLength);
		glVertex3f(i - MAXLINES * linesLength, 0, MAXLINES * linesLength);
		glVertex3f(-MAXLINES * linesLength, 0, i - MAXLINES * linesLength);
		glVertex3f(MAXLINES * linesLength, 0, i - MAXLINES * linesLength);
	}
	glEnd();
	glLineWidth(lineWidth);

}

