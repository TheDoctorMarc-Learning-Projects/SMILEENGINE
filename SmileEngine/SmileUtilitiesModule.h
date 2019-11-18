#pragma once
#include "SmileModule.h"
#include "SmileSetup.h"
#include "Utility.h"
#include <map>

class SmileUtilitiesModule : public SmileModule
{
public:
	SmileUtilitiesModule(SmileApp* app, bool start_enabled = true);
	~SmileUtilitiesModule();

public: 

	bool Start();
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

public: 
	Utility* GetUtility(std::string name); 
	
private:
	std::map <std::string, Utility*> utilities;
};
