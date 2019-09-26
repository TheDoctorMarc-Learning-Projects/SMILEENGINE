#include "SmileSetup.h"
#include "SmileApp.h"
#include "SmileUtilitiesModule.h"

#include <random>
 
#include "rapidjson/include/rapidjson/writer.h"
#include "rapidjson/include/rapidjson/reader.h"
#include "rapidjson/include/rapidjson/istreamwrapper.h"
#include "rapidjson/include/rapidjson/stringbuffer.h"

//#include <iostream>
#include <fstream>

 
SmileUtilitiesModule::SmileUtilitiesModule(SmileApp* app, bool start_enabled) : SmileModule(app, start_enabled)
{
}

SmileUtilitiesModule::~SmileUtilitiesModule()
{}

// -----------------------------------------------------------------
bool SmileUtilitiesModule::Start()
{
	// PCG

	// Seed with a real random value, if available
	pcg_extras::seed_seq_from<std::random_device> seed_source;

	// Make a random number engine 
	rng.seed(seed_source); 


	// JSON
	rapidjson::StringBuffer s;
	rapidjson::Writer<rapidjson::StringBuffer> writer(s);

	writer.StartObject();               // Between StartObject()/EndObject(), 
	writer.Key("hello");                // output a key,
	writer.String("world");             // follow by a value.
	writer.Key("t");
	writer.Bool(true);
	writer.Key("f");
	writer.Bool(false);
	writer.Key("n");
	writer.Null();
	writer.Key("i");
	writer.Uint(123);
	writer.Key("pi");
	writer.Double(3.1416);
	writer.Key("a");
	writer.StartArray();                // Between StartArray()/EndArray(),
	for (unsigned i = 0; i < 4; i++)
		writer.Uint(i);                 // all values are elements of the array.
	writer.EndArray();
	writer.EndObject();

	// {"hello":"world","t":true,"f":false,"n":null,"i":123,"pi":3.1416,"a":[0,1,2,3]}
	LOG("Generated JSON file string ----------> %s", s.GetString()); 

	std::ofstream ofs("./test.json", std::ofstream::out);
	ofs << s.GetString();
	ofs.close();




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

	// dirty JSON tests
	if (App->input->GetKey(SDL_SCANCODE_O) == KEY_DOWN)
		LOG("JSON file string ----------> %s", ConvertJSONToChar(ReadJSONFile("./test.json")));
		

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


rapidjson::Document SmileUtilitiesModule::ReadJSONFile(const char* path)
{
	// read the path and convert it to JSON Document
	std::ifstream ifs(path);
	rapidjson::IStreamWrapper isw(ifs);
	rapidjson::Document d;
	d.ParseStream(isw);

	return d; 
}


std::string SmileUtilitiesModule::ConvertJSONToChar(rapidjson::Document d)
{
	// Stringify the DOM
	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	d.Accept(writer);

	return std::string(buffer.GetString());
}
 
