#pragma once
#include "SmileModule.h"
#include "SmileSetup.h"
#include "Primitive.h"

class SmileScene : public SmileModule
{
public:
	SmileScene(SmileApp* app, bool start_enabled = true);
	~SmileScene();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

public:
	
};
