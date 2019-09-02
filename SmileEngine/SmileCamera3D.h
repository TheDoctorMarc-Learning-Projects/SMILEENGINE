#pragma once
#include "SmileModule.h"
#include "SmileSetup.h"



class SmileCamera3D : public SmileModule
{
public:
	SmileCamera3D(SmileApp* app, bool start_enabled = true);
	~SmileCamera3D();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

private: 
};