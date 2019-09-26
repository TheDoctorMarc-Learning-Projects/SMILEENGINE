#pragma once
#include "SmileModule.h"
#include "SmileSetup.h"
#include <variant>

#include "pcg/include/pcg_random.hpp"

#include "rapidjson/include/rapidjson/document.h"

class SmileUtilitiesModule : public SmileModule
{
public:
	SmileUtilitiesModule(SmileApp* app, bool start_enabled = true);
	~SmileUtilitiesModule();

	bool Start();
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	// random, PCG
	std::variant<int, float> GetRandomValue(std::variant<int, float> start = 0,
		std::variant<int, float> end = INT_MAX);

	// JSON 
	rapidjson::Document ReadJSONFile(const char* path);
	std::string ConvertJSONToChar(rapidjson::Document d);

private:

	pcg32 rng; 
};
