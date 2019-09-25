#include "SmileSetup.h"
#include "SmileApp.h"
#include "SmileUtilitiesModule.h"

#include <random>
#include <typeinfo>

SmileUtilitiesModule::SmileUtilitiesModule(SmileApp* app, bool start_enabled) : SmileModule(app, start_enabled)
{
}

SmileUtilitiesModule::~SmileUtilitiesModule()
{}

// -----------------------------------------------------------------
bool SmileUtilitiesModule::Start()
{
	// Seed with a real random value, if available
	pcg_extras::seed_seq_from<std::random_device> seed_source;

	// Make a random number engine 
	rng.seed(seed_source); 

	// Choose a random mean 

	return true;
}

// -----------------------------------------------------------------
bool SmileUtilitiesModule::CleanUp()
{
	return true;
}

/*// -----------------------------------------------------------------
update_status SmileUtilitiesModule::PreUpdate(float dt)
{
	return UPDATE_CONTINUE;
}


// -----------------------------------------------------------------
update_status SmileUtilitiesModule::Update(float dt)
{
	
	return UPDATE_CONTINUE;
}

// -----------------------------------------------------------------
update_status SmileUtilitiesModule::PostUpdate(float dt)
{
	return UPDATE_CONTINUE;
}*/



std::variant<int, float> SmileUtilitiesModule::GetRandomValue(std::variant<int, float> start,
	std::variant<int, float> end)
{
	if (typeid(start) != typeid(end))
		return NAN; 
	 
	std::uniform_int_distribution<std::variant<int, float>> uniform_dist(start, end);
	std::variant<int, float> number = uniform_dist(rng);

	return number; 
}

