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
		int scale = rapidjson::GetValueByPointer(doc, "/Window/Scale")->GetInt(); 
		int width = rapidjson::GetValueByPointer(doc, "/Window/Width")->GetInt() * scale;
		int height = rapidjson::GetValueByPointer(doc, "/Window/Height")->GetInt() * scale;
		bool fullscreen = rapidjson::GetValueByPointer(doc, "/Window/Fullscreen")->GetBool();
		bool borderless = rapidjson::GetValueByPointer(doc, "/Window/Borderless")->GetBool();
		bool resizable = rapidjson::GetValueByPointer(doc, "/Window/Resizable")->GetBool();
		bool fullscreenDesktpp = rapidjson::GetValueByPointer(doc, "/Window/FullDesktop")->GetBool();

		//Create window
		Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;

		//Use OpenGL 2.1
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

		if(fullscreen == true)
		{
			flags |= SDL_WINDOW_FULLSCREEN;
		}

		if(resizable == true)
		{
			flags |= SDL_WINDOW_RESIZABLE;
		}

		if(borderless == true)
		{
			flags |= SDL_WINDOW_BORDERLESS;
		}

		if(fullscreenDesktpp == true)
		{
			flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
		}

		window = SDL_CreateWindow(TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, flags);

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
	{
		SDL_DestroyWindow(window);
	}

	//Quit SDL subsystems
	SDL_Quit();
	return true;
}

void SmileWindow::SetTitle(const char* title)
{
	SDL_SetWindowTitle(window, title);
}

 