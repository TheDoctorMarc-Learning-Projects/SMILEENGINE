#pragma once
#include "Module.h"

class ModulePhysics3D : public Module
{
public:
	ModulePhysics3D(SmileApp* app, bool start_enabled = true);
	~ModulePhysics3D();

	bool Init();
	bool Start();
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

public:

}; 