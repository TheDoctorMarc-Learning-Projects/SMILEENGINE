#pragma once
#include "SmileModule.h"
#include "SmileSetup.h"
#include "SmileFBX.h"
#include "GameObject.h"
#include "ComponentMesh.h"
#include "ComponentCamera.h"
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
	ComponentCamera* debugCamera = nullptr; 
	ComponentCamera* gameCamera = nullptr;

	int lineWidth = 1;
	float linesLength = 0.5f;

};
