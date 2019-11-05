#pragma once
#include "SmileModule.h"
#include "SmileSetup.h"
#include "SmileFBX.h"
#include "GameObject.h"
#include "ComponentMesh.h"
#include "GameObjectCamera.h"
#include <vector>
#define MAXLINES 30
class SmileScene : public SmileModule
{
public:
	SmileScene(SmileApp* app, bool start_enabled = true);
	~SmileScene();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

private: 
	void DrawGrid(); 

public:
	GameObject* rootObj = nullptr;
	GameObject* selectedObj = nullptr; 
	ComponentMesh* selected_mesh = nullptr;
	GameObjectCamera* debugCamera = nullptr; 

	int lineWidth = 1;
	float linesLength = 0.5f;
	/*// test
	par_shapes_mesh* testCube = nullptr;
	uint* vertexIDarray = nullptr; 
	uint* indexIDarray = nullptr;
	uint vertexID = 0; 
	uint indexID = vertexID; */

};
