#pragma once
#include "SmileModule.h"

class SmilePlayer : public SmileModule
{
public:
	SmilePlayer(SmileApp* app, bool start_enabled = true);
	virtual ~SmilePlayer();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

public:

};