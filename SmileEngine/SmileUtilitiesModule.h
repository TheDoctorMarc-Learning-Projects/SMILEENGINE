#pragma once
#include "SmileModule.h"
#include "SmileSetup.h"
#include <variant>
#include <map>
#include <any>

#include "pcg/include/pcg_random.hpp"

#include "rapidjson/include/rapidjson/document.h"

#define JSONFileMaxBufferSize 5000

typedef void (*functionA) (std::any);
typedef void (*functionB) (std::variant<std::vector<int>, std::vector<float>>);

void ShowCursor(std::any);
void ChangeWindowFullScreen(std::any);
void SetWindowSize(std::variant<std::vector<int>, std::vector<float>>);

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
	void ParseJSONFile(const char* path, rapidjson::Document& fill);
	void FillFunctionsMap();


private: 
	void SetGameConfigParameters(const rapidjson::Document& d);

private:
	pcg32 rng;
	std::map<std::string, functionA> singleParamConfigFunctionMap;
	std::map<std::string, functionB> multipleParamConfigFunctionMap;

};
