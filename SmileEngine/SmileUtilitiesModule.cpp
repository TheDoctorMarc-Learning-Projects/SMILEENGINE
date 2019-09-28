#include "SmileSetup.h"
#include "SmileApp.h"
#include "SmileUtilitiesModule.h"
#include "SmileWindow.h"

#include <random>
 
#include "rapidjson/include/rapidjson/writer.h"
#include "rapidjson/include/rapidjson/reader.h"
#include "rapidjson/include/rapidjson/stringbuffer.h"


#include "rapidjson/include/rapidjson/filereadstream.h"

//#include <iostream>
#include <fstream>

 
SmileUtilitiesModule::SmileUtilitiesModule(SmileApp* app, bool start_enabled) : SmileModule(app, start_enabled)
{
	FillFunctionsMap(); 
}

SmileUtilitiesModule::~SmileUtilitiesModule()
{
	singleParamConfigFunctionMap.clear();
	multipleParamConfigFunctionMap.clear(); 
}

// -----------------------------------------------------------------
bool SmileUtilitiesModule::Start()
{
	// PCG

	// Seed with a real random value, if available
	pcg_extras::seed_seq_from<std::random_device> seed_source;

	// Make a random number engine 
	rng.seed(seed_source); 

	
	// JSON
	rapidjson::Document doc; 
	ParseJSONFile("config.json", doc);
	SetGameConfigParameters(doc);

	return true;
}

// -----------------------------------------------------------------
bool SmileUtilitiesModule::CleanUp()
{
	return true;
}

// -----------------------------------------------------------------
update_status SmileUtilitiesModule::PreUpdate(float dt)
{
	// dirty random tests
	if (App->input->GetKey(SDL_SCANCODE_R) == KEY_DOWN)
		LOG("Random value ----------> %f", std::get<float>(GetRandomValue(0.F, 500.F)));

	if (App->input->GetKey(SDL_SCANCODE_T) == KEY_DOWN)
		LOG("Random value ----------> %i", std::get<int>(GetRandomValue(0, 500)));

	if (App->input->GetKey(SDL_SCANCODE_Y) == KEY_DOWN)
		LOG("Random value ----------> %f", std::get<float>(GetRandomValue(0, 500.F)));


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


// ----------------------------------------------------------------- [Get a random int or float number]  

std::variant<int, float> SmileUtilitiesModule::GetRandomValue(std::variant<int, float> start,
	std::variant<int, float> end)
{
	if (start.index() != end.index())
		return NAN; 

	std::variant<int, float> number;
	std::variant<int, float> test = 1;
 
	if (start.index() == test.index())  // the index of a variant variable translates to a type (int, float, etc) 
	{
		std::uniform_int_distribution <int> uniform_dist(std::get<int>(start), std::get<int>(end));
		number = uniform_dist(rng);
	}
		
	else
	{
		std::uniform_real_distribution <float> uniform_dist(std::get<float>(start), std::get<float>(end));
		number = uniform_dist(rng);
	}
		
	return number; 
}

// ----------------------------------------------------------------- [Open and return a JSON file]  
void SmileUtilitiesModule::ParseJSONFile(const char* path, rapidjson::Document& fill)
{
	// 1) read the path and convert it to JSON Document
	FILE* pFile = fopen(path, "rb");
	char buffer[JSONFileMaxBufferSize];
	rapidjson::FileReadStream is(pFile, buffer, sizeof(buffer));
	fill.ParseStream<0, rapidjson::UTF8<>, rapidjson::FileReadStream>(is);
}

 
// ----------------------------------------------------------------- [Test to use JSON genericaly to call various config functions] 
void SmileUtilitiesModule::SetGameConfigParameters(const rapidjson::Document& d)
{
	for (auto& obj : d.GetObject())
	{
		const char* name = obj.name.GetString();
		std::string strName = name;
		std::any value; 
		std::variant<std::vector<int>, std::vector<float>> arrayValues; 
		bool isArray = false; 

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

			multipleParamConfigFunctionMap[strName](arrayValues);

			if (isIntArray)
				std::get<std::vector<int>>(arrayValues).clear(); 
			else
				std::get<std::vector<float>>(arrayValues).clear();
		}

		if(isArray == false)
			singleParamConfigFunctionMap[strName](value);
	

	}

}

// ----------------------------------------------------------------- [Test to use JSON genericaly to call various config functions] 

void SmileUtilitiesModule::FillFunctionsMap()
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

/*void ChangeWindowSize(std::any value)
{
	int[] a = std::any_cast<int[]>(value);
	App->renderer3D->OnResize(a, b)
}*/

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