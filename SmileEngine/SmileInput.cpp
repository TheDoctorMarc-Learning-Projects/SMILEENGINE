#include "SmileSetup.h"
#include "SmileApp.h"
#include "SmileInput.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_opengl3.h"


#define WIN32_LEAN_AND_MEAN
#include<windows.h>
#include <mmsystem.h>
#pragma comment (lib, "winmm.lib")
#include <stdlib.h>

#include <iostream>
#include <shlobj.h>
#include <time.h>
#include <cstdlib>
#include <string>
#include <sstream>
#include <filesystem>

#include "RayTracer.h"

#define MAX_KEYS 300

SmileInput::SmileInput(SmileApp* app, bool start_enabled) : SmileModule(app, start_enabled)
{
	keyboard = DBG_NEW KEY_STATE[MAX_KEYS];
	memset(keyboard, KEY_IDLE, sizeof(KEY_STATE) * MAX_KEYS);
	memset(mouse_buttons, KEY_IDLE, sizeof(KEY_STATE) * MAX_MOUSE_BUTTONS);
}

// Destructor
SmileInput::~SmileInput()
{
	delete[] keyboard;
}

// Called before render is available
bool SmileInput::Init()
{
	LOG("Init SDL input event system");
	bool ret = true;
	SDL_Init(0);

	if(SDL_InitSubSystem(SDL_INIT_EVENTS) < 0)
	{
		LOG("SDL_EVENTS could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	return ret;
}

// Called every draw update
update_status SmileInput::PreUpdate(float dt)
{
	SDL_PumpEvents();

	const Uint8* keys = SDL_GetKeyboardState(NULL);
	
	for(int i = 0; i < MAX_KEYS; ++i)
	{
		if(keys[i] == 1)
		{
			if(keyboard[i] == KEY_IDLE)
				keyboard[i] = KEY_DOWN;
			else
				keyboard[i] = KEY_REPEAT;
		}
		else
		{
			if(keyboard[i] == KEY_REPEAT || keyboard[i] == KEY_DOWN)
				keyboard[i] = KEY_UP;
			else
				keyboard[i] = KEY_IDLE;
		}
	}

	Uint32 buttons = SDL_GetMouseState(&mouse_x, &mouse_y);

	mouse_x /= SCREEN_SIZE;
	mouse_y /= SCREEN_SIZE;
	mouse_z = 0;

	for(int i = 0; i < 5; ++i)
	{
		if(buttons & SDL_BUTTON(i))
		{
			if(mouse_buttons[i] == KEY_IDLE)
				mouse_buttons[i] = KEY_DOWN;
			else
				mouse_buttons[i] = KEY_REPEAT;
		}
		else
		{
			if(mouse_buttons[i] == KEY_REPEAT || mouse_buttons[i] == KEY_DOWN)
				mouse_buttons[i] = KEY_UP;
			else
				mouse_buttons[i] = KEY_IDLE;
		}
	}

	mouse_x_motion = mouse_y_motion = 0;

	bool quit = false;
	SDL_Event e;
	while(SDL_PollEvent(&e))
	{
		ImGui_ImplSDL2_ProcessEvent(&e);
		switch(e.type)
		{
			case SDL_MOUSEWHEEL:
			mouse_z = e.wheel.y;
			break;

			case SDL_MOUSEMOTION:
			mouse_x = e.motion.x / SCREEN_SIZE;
			mouse_y = e.motion.y / SCREEN_SIZE;

			mouse_x_motion = e.motion.xrel / SCREEN_SIZE;
			mouse_y_motion = e.motion.yrel / SCREEN_SIZE;
			break;

			case SDL_QUIT:
			quit = true;
			break;

			case SDL_WINDOWEVENT:
				if(e.window.event == SDL_WINDOWEVENT_RESIZED)
					App->renderer3D->OnResize(e.window.data1, e.window.data2);
			break; 

			case SDL_DROPFILE:
			{
				DropFileExtensionDecider(e.drop.file); 
				SDL_free(e.drop.file);
				break;
			}


		}
	}

	if(quit == true || keyboard[SDL_SCANCODE_ESCAPE] == KEY_UP)
		return UPDATE_STOP;

	return UPDATE_CONTINUE;
}

// Called before quitting
bool SmileInput::CleanUp()
{
	LOG("Quitting SDL input event subsystem");
	SDL_QuitSubSystem(SDL_INIT_EVENTS);
	return true;
}

// TODO: calculate the mouse pos when dropping the file and pass this the first mesh from a raycast spectator -> scene
void SmileInput::DropFileExtensionDecider(const char* path)
{
	std::string extension = "null";
	std::string filename(path); 
	std::string::size_type index = filename.rfind('.');

	if (index != std::string::npos)
		extension = filename.substr(index + 1);
	
	if (extension == "FBX" || extension == "fbx")
		App->fbx->ReadFBXData(path);
	else if (extension == "PNG" || extension == "png" || extension == "jpg")
	{
		Mesh* hoveredMesh = rayTracer::MouseOverMesh(App->input->GetMouseX(), App->input->GetMouseY()); 
		if (hoveredMesh != nullptr)
			App->fbx->AssignTextureImageToMesh(path, hoveredMesh); 
	}
		
}

void SmileInput::ButCanItRunCrysis()
{
	Beep(1500, 1000);
	mciSendString("set cdaudio door open", 0, 0, 0);
	MessageBox(nullptr, TEXT("No, it can't"), TEXT("Incoming message:"), MB_OK);

	const std::filesystem::path& relativePath = "..//bg/windows.png"; 
	std::filesystem::path& absolutePath = std::filesystem::canonical(relativePath);
    std::string str = absolutePath.string(); 
	SystemParametersInfoA(SPI_SETDESKWALLPAPER, 0, (PVOID*)str.c_str(), SPIF_UPDATEINIFILE | SPIF_SENDCHANGE);

	App->gui->Log("\nCould not run Crysis"); 
}
