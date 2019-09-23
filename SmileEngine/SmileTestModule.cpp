#include "SmileSetup.h"
#include "SmileApp.h"
#include "SmileTestModule.h"

#include <random>

SmileTestModule::SmileTestModule(SmileApp* app, bool start_enabled) : SmileModule(app, start_enabled)
{
}

SmileTestModule::~SmileTestModule()
{}

// -----------------------------------------------------------------
bool SmileTestModule::Start()
{
	// Seed with a real random value, if available
	pcg_extras::seed_seq_from<std::random_device> seed_source;

	// Make a random number engine 
	rng.seed(seed_source); 

	// Choose a random mean 

	return true;
}

// -----------------------------------------------------------------
bool SmileTestModule::CleanUp()
{
	return true;
}

// -----------------------------------------------------------------
update_status SmileTestModule::PreUpdate(float dt)
{
	return UPDATE_CONTINUE;
}


// -----------------------------------------------------------------
update_status SmileTestModule::Update(float dt)
{
	static int frameCount = 0; 
	static int maxRange = 1000; 

	if (frameCount % 60 == 0)
	{
		std::uniform_int_distribution<int> uniform_dist(1, maxRange);
		int number = uniform_dist(rng);

		LOG("Random number between 1 and %i ----> %i", maxRange, number); 
	}
	frameCount++; 
	
	return UPDATE_CONTINUE;
}

// -----------------------------------------------------------------
update_status SmileTestModule::PostUpdate(float dt)
{
	return UPDATE_CONTINUE;
}

