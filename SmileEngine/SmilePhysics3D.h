#pragma once
#include "SmileModule.h"

class SmilePhysics3D : public SmileModule
{
public:
	SmilePhysics3D(SmileApp* app, bool start_enabled = true);
	~SmilePhysics3D();

	bool Init();
	bool Start();
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

public:

}; 