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
	configFunctionsMap.clear(); 
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
	ReturnJSONFile("config.json", doc);
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
void SmileUtilitiesModule::ReturnJSONFile(const char* path, rapidjson::Document& fill)
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

		if (obj.value.IsBool())
		{
			bool v = d[name].GetBool();
			value = std::make_any<bool>(v);
			configFunctionsMap[strName](value);
		}
		else if (obj.value.IsInt())
		{
			int v = d[name].GetInt();
			value = std::make_any<int>(v);
			configFunctionsMap[strName](value);
		}
		else if (obj.value.IsFloat())
		{
			float v = d[name].GetFloat();
			value = std::make_any<float>(v);
			configFunctionsMap[strName](value);
		}

		// TODO: arrays

	}

}

// ----------------------------------------------------------------- [Test to use JSON genericaly to call various config functions] 

void SmileUtilitiesModule::FillFunctionsMap()
{

	configFunctionsMap =
	{
		//{"window", &ChangeWindowSize},
		{"cursor", &ShowCursor},
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