#pragma once
#include "Utility.h"
#include <variant>
#include <vector>
#include <map>
#include <any>
#include "rapidjson/include/rapidjson/document.h"

#define JSONFileMaxBufferSize 5000

typedef void (*functionA) (std::any); // JSON objects will trigger this function with a bool, inf, float etc argument
typedef void (*functionB) (std::variant<std::vector<int>, std::vector<float>>); // same as above but with an array argument

void ShowCursor(std::any);
void ChangeWindowFullScreen(std::any);
void SetWindowSize(std::variant<std::vector<int>, std::vector<float>>);

class JSONParser : public Utility
{
public: 
	bool Initialize(); 
    bool ShutDown();

private: 
	void ParseJSONFile(const char* path, rapidjson::Document& fill);
	void FillFunctionsMap();
	void SetGameConfigParameters(const rapidjson::Document& d);

private:
	std::map<std::string, functionA> singleParamConfigFunctionMap;
	std::map<std::string, functionB> multipleParamConfigFunctionMap;
}; 
