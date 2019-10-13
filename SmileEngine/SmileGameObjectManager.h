#pragma once

#include "SmileModule.h"
#include "ComponentMesh.h"

class SmileGameObjectManager : public SmileModule
{
public:
	SmileGameObjectManager(SmileApp* app, bool start_enabled = true);
	~SmileGameObjectManager();

	bool Start(); 
	update_status Update(float dt);
	bool CleanUp(); 

	void AssignTextureImageToMesh(const char* path, ComponentMesh* mesh)
	{ 
		mesh->AssignTexture(path); 
	};
};