#include "SmileSetup.h"
#include "SmileApp.h"
#include "JSONParser.h"
#include "SmileWindow.h"

#include "rapidjson/include/rapidjson/writer.h"
#include "rapidjson/include/rapidjson/reader.h"
#include "rapidjson/include/rapidjson/stringbuffer.h"


#include "rapidjson/include/rapidjson/filereadstream.h"

#include <fstream>


bool JSONParser::Initialize()
{
	FillFunctionsMap();

	rapidjson::Document doc;
	ParseJSONFile("config.json", doc);
	SetGameConfigParameters(doc);
	
	return true; 
}

// -----------------------------------------------------------------
bool JSONParser::ShutDown()
{
	singleParamConfigFunctionMap.clear();
	multipleParamConfigFunctionMap.clear();

	return true; 
}

// ----------------------------------------------------------------- [Open a JSON file and copy it to a Document variable]  
void JSONParser::ParseJSONFile(const char* path, rapidjson::Document& fill)
{
	// 1) read the path and convert it to JSON Document
	FILE* pFile = fopen(path, "rb");
	char buffer[JSONFileMaxBufferSize];
	rapidjson::FileReadStream is(pFile, buffer, sizeof(buffer));
	fill.ParseStream<0, rapidjson::UTF8<>, rapidjson::FileReadStream>(is);
}

// ----------------------------------------------------------------- [Read JSON objects and call their functions] 
void JSONParser::SetGameConfigParameters(const rapidjson::Document& d)
{
	for (auto& obj : d.GetObject())
	{
		const char* name = obj.name.GetString();
		std::string strName = name;
		std::any value;
		std::variant<std::vector<int>, std::vector<float>> arrayValues;
		bool isArray = false;

		// Normal 1 element objects
		if (obj.value.IsBool())
		{
			bool v = d[name].GetBool();
			value = std::make_any<bool>(v);
		}
		else if (obj.value.IsInt())
		{
			int v = d[name].GetInt();
			value = std::make_any<int>(v);
		}
		else if (obj.value.IsFloat())
		{
			float v = d[name].GetFloat();
			value = std::make_any<float>(v);
		}

		// Arrays
		else if (obj.value.IsArray())
		{
			isArray = true;
			bool isIntArray = true;
			std::vector<int> iVec;
			std::vector<float> fVec;

			for (auto& v : d[name].GetArray())
			{
				if (v.IsInt())
					iVec.push_back(v.GetInt());
				else if (v.IsFloat())
				{
					isIntArray = false;
					fVec.push_back(v.GetFloat());
				}

			}

			if (isIntArray == true)
			{
				arrayValues = iVec;
				iVec.clear();
			}

			else if (isIntArray == false)
			{
				arrayValues = fVec;
				fVec.clear();
			}

			multipleParamConfigFunctionMap[strName](arrayValues);  // call a function that recieves multiple element data

			if (isIntArray)
				std::get<std::vector<int>>(arrayValues).clear();
			else
				std::get<std::vector<float>>(arrayValues).clear();
		}

		if (isArray == false)
			singleParamConfigFunctionMap[strName](value);  // call a function that recieves 1 element data 

	}

}

// ----------------------------------------------------------------- [Use JSON genericaly to call various config functions] 
void JSONParser::FillFunctionsMap()
{

	singleParamConfigFunctionMap =
	{
		{"cursor", &ShowCursor},
	};

	multipleParamConfigFunctionMap =
	{
		{"window", &SetWindowSize},
	};

}

void ShowCursor(std::any value)
{
	int show = (std::any_cast<bool>(value) == true) ? 1 : 0;
	SDL_ShowCursor(show);
}

void SetWindowSize(std::variant<std::vector<int>, std::vector<float>> values)
{
	std::vector<int> realValues = std::get<std::vector<int>>(values);
	App->window->SetWindowSize(realValues.at(0), realValues.at(1));
}
