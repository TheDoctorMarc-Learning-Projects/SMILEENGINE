#include "SmileSetup.h"
#include "SmileApp.h"
#include "SmileWindow.h"
#include "SmileUtilitiesModule.h"
#include "JSONParser.h"

SmileWindow::SmileWindow(SmileApp* app, bool start_enabled) : SmileModule(app, start_enabled)
{
	window = NULL;
	screen_surface = NULL;
}

// Destructor
SmileWindow::~SmileWindow()
{
	windowVariables.map.clear(); 
}

// Called before render is available
bool SmileWindow::Init()
{
	LOG("Init SDL window & surface");
	bool ret = true;

	rapidjson::Document doc;
	dynamic_cast<JSONParser*>(App->utilities->GetUtility("JSONParser"))->ParseJSONFile("config.json", doc);

	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		LOG("SDL_VIDEO could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	else
	{
		windowVariables.Scale = rapidjson::GetValueByPointer(doc, "/Window/0/Scale")->GetInt(); 
		windowVariables.Width = rapidjson::GetValueByPointer(doc, "/Window/0/Width")->GetInt() * windowVariables.Scale;
		windowVariables.Height = rapidjson::GetValueByPointer(doc, "/Window/0/Height")->GetInt() * windowVariables.Scale;
		windowVariables.Fullscreen = rapidjson::GetValueByPointer(doc, "/Window/0/Fullscreen")->GetBool();
		windowVariables.Borderless = rapidjson::GetValueByPointer(doc, "/Window/0/Borderless")->GetBool();
		windowVariables.Resizable = rapidjson::GetValueByPointer(doc, "/Window/0/Resizable")->GetBool();
		windowVariables.FullDesktop = rapidjson::GetValueByPointer(doc, "/Window/0/FullDesktop")->GetBool();

		windowVariables.map =
		{	
			{"Brightness", windowVariables.Brightness},
			{"Width", windowVariables.Width},
			{"Height", windowVariables.Height},
			{"Scale", windowVariables.Scale},
			{"Fullscreen", windowVariables.Fullscreen},
			{"Borderless", windowVariables.Borderless},
			{"Resizable", windowVariables.Resizable},
			{"FullDesktop", windowVariables.FullDesktop},
		};

		//Create window
		Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;

		//Use OpenGL 2.1
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

		if(windowVariables.Fullscreen == true)
		{
			flags |= SDL_WINDOW_FULLSCREEN;
		}

		if(windowVariables.Resizable == true)
		{
			flags |= SDL_WINDOW_RESIZABLE;
		}

		if(windowVariables.Borderless == true)
		{
			flags |= SDL_WINDOW_BORDERLESS;
		}

		if(windowVariables.FullDesktop == true)
		{
			flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
		}

		window = SDL_CreateWindow(TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, windowVariables.Width, windowVariables.Height, flags);

		if(window == NULL)
		{
			LOG("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			ret = false;
		}
		else
		{
			//Get window surface
			screen_surface = SDL_GetWindowSurface(window);
		}
	}

	return ret;
}

// Called before quitting
bool SmileWindow::CleanUp()
{
	LOG("Destroying SDL window and quitting all SDL systems");

	//Destroy window
	if(window != NULL)
		SDL_DestroyWindow(window);

	//Free surface
	if (screen_surface != NULL)
		SDL_FreeSurface(screen_surface);

	//Quit SDL subsystems
	SDL_Quit();
	return true;
}

void SmileWindow::SetTitle(const char* title)
{
	SDL_SetWindowTitle(window, title);
}

 