#include "SmileSetup.h"
#include "SmileApp.h"
#include "SmileUtilitiesModule.h"
 
#include "JSONParser.h"
#include "RNG.h"
 
SmileUtilitiesModule::SmileUtilitiesModule(SmileApp* app, bool start_enabled) : SmileModule(app, start_enabled)
{

}

SmileUtilitiesModule::~SmileUtilitiesModule()
{

}

// -----------------------------------------------------------------
bool SmileUtilitiesModule::Start()
{
	utilities.insert(std::pair("JSONParser", DBG_NEW JSONParser())); 
	utilities.insert(std::pair("RNG", DBG_NEW RNG()));
 
	for (auto& utility : utilities)
		utility.second->Initialize(); 

	return true;
}

// -----------------------------------------------------------------
bool SmileUtilitiesModule::CleanUp()
{
	for (auto& utility : utilities)
	{
		utility.second->ShutDown();
		RELEASE(utility.second); 
	}
	
	utilities.clear(); 

	return true;
}

// -----------------------------------------------------------------
update_status SmileUtilitiesModule::PreUpdate(float dt)
{ 


	// dirty random tests
	if (App->input->GetKey(SDL_SCANCODE_R) == KEY_DOWN)
		LOG("Random value ----------> %f", std::get<float>(dynamic_cast<RNG*>(GetUtility("RNG"))->GetRandomValue(0.F, 500.F)));

	if (App->input->GetKey(SDL_SCANCODE_T) == KEY_DOWN)
		LOG("Random value ----------> %i", std::get<int>(dynamic_cast<RNG*>(GetUtility("RNG"))->GetRandomValue(0, 500)));

	if (App->input->GetKey(SDL_SCANCODE_Y) == KEY_DOWN)
		LOG("Random value ----------> %f", std::get<float>(dynamic_cast<RNG*>(GetUtility("RNG"))->GetRandomValue(0, 500.F)));


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
}

Utility* SmileUtilitiesModule::GetUtility(std::string name)
{
	auto item = utilities.find(name);
	if (item != utilities.end())
		return utilities.at(name);
	return nullptr;
}

