#pragma once
#include "SmileModule.h"

class SmileSmilePlayer : public SmileModule
{
public:
	SmileSmilePlayer(SmileApp* app, bool start_enabled = true);
	virtual ~SmileSmilePlayer();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

public:

};