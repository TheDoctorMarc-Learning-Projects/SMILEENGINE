#pragma once
#include "SmileModule.h"
#include "SmileSetup.h"

#include "pcg/include/pcg_random.hpp"

class SmileTestModule : public SmileModule
{
public:
	SmileTestModule(SmileApp* app, bool start_enabled = true);
	~SmileTestModule();

	bool Start();
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

private:

	pcg32 rng; 
};
