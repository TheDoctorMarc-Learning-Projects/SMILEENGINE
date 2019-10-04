#pragma once
#include "SmileModule.h"
#include "SmileSetup.h"

#include "parshapes/par_shapes.h"

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

	// test
	par_shapes_mesh* testCube = nullptr;
	uint* vertexIDarray = nullptr; 
	uint* indexIDarray = nullptr;
	uint vertexID = 0; 
	uint indexID = vertexID; 
};
