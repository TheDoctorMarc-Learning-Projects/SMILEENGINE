#pragma once
#include "Utility.h"
#include <variant>
#include <vector>
#include <map>
#include <any>
#include "rapidjson/include/rapidjson/document.h"
#include "rapidjson/include/rapidjson/pointer.h"

// so you can write in other files  
#include "rapidjson/include/rapidjson/writer.h"
#include "rapidjson/include/rapidjson/stringbuffer.h"

#define JSONFileMaxBufferSize 5000

typedef void (*functionA) (std::any); // JSON objects will trigger this function with a bool, inf, float etc argument
typedef void (*functionB) (std::variant<std::vector<int>, std::vector<float>>); // same as above but with an array argument

// delcare functions if needed 

class JSONParser : public Utility
{
public: 
	bool Initialize(); 
    bool ShutDown();

	void ParseJSONFile(const char* path, rapidjson::Document& fill);

private: 
	void FillFunctionsMap();
	void ExecuteSimpleJSONFunctions(const rapidjson::Document& d);

private:
	std::map<std::string, functionA> singleParamConfigFunctionMap;
	std::map<std::string, functionB> multipleParamConfigFunctionMap;
}; 
