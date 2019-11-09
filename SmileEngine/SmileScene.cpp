#define PAR_SHAPES_IMPLEMENTATION
#include "parshapes/par_shapes.h"

#include "SmileSetup.h"
#include "SmileApp.h"
#include "SmileScene.h"
#include "ComponentTransform.h"
/*#include <gl/GL.h>
//#include <gl/GLU.h>*/

#include "Glew/include/GL/glew.h"
#include "ComponentCamera.h"
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
	GameObject* debugCameraObj = DBG_NEW GameObject(DBG_NEW ComponentTransform(float3(0, 20, 10)), "Debug Camera", rootObj);
	debugCamera = DBG_NEW ComponentCamera(debugCameraObj, vec3(0, 0, 9)); 
	debugCameraObj->AddComponent(debugCamera);

	// Game Camera
	GameObject* gameCameraObj = DBG_NEW GameObject(DBG_NEW ComponentTransform(float3(0, 0, 7)), "Game Camera", rootObj);
	renderingData data; 
	data.pFarDist = 10.f; 
	gameCamera = DBG_NEW ComponentCamera(gameCameraObj, vec3(0, 0, 0), data); 
	gameCameraObj->AddComponent(gameCamera);
	

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

