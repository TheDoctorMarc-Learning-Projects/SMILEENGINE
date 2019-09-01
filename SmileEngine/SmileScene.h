#pragma once
#include "SmileModule.h"
#include "p2DynArray.h"
#include "SmileSetup.h"
#include "Primitive.h"

class SmileSmileScene : public SmileModule
{
public:
	SmileSmileScene(SmileApp* app, bool start_enabled = true);
	~SmileSmileScene();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

public:
	
};
