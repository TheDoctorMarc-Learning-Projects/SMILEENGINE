#pragma once
#include "SmileModule.h"
#include "SmileSetup.h"
#include "SmileFBX.h"
#include "GameObject.h"
#include "ComponentMesh.h"
#include <vector>

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
	void UpdateGameObjects();
	std::vector<GameObject*> objects;
	GameObject* rootObj = nullptr;
	GameObject* selectedObj = nullptr; 
	ComponentMesh* selected_mesh = nullptr;

	/*// test
	par_shapes_mesh* testCube = nullptr;
	uint* vertexIDarray = nullptr; 
	uint* indexIDarray = nullptr;
	uint vertexID = 0; 
	uint indexID = vertexID; */

};
