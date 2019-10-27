#ifndef __SmileWindow_H__
#define __SmileWindow_H__

#include "SmileModule.h"
#include "SDL/include/SDL.h"

#include <map>
#include <variant>
#include <string>

#define NAMEOF(variable) ((decltype(&variable))nullptr, #variable)

class SmileApp;

struct windowParams
{
	int Width, Height, Scale, Brightness;
	bool Fullscreen, Borderless, Resizable, FullDesktop;

	std::map<std::string, std::variant<int, bool>> map; 

};

class SmileWindow : public SmileModule
{
public:

	SmileWindow(SmileApp* app, bool start_enabled = true);

	// Destructor
	virtual ~SmileWindow();

	bool Init();
	bool CleanUp();

	void SetTitle(const char* title);

public: 
	void SetWindowSize(int w, int h)
	{
		SDL_SetWindowSize(window, w, h);
		windowVariables.Width = w; 
		windowVariables.Height = h;
	};
	void setBrightness(int bg)
	{
		SDL_SetWindowBrightness(window, bg);
		windowVariables.Brightness = bg;
	}

	std::variant<int, bool> GetWindowParameter(std::string name)
	{
		auto item = windowVariables.map.find(name);
		if (item != windowVariables.map.end())
			return windowVariables.map.at(name);
		return (int)NAN;
	}; 

	bool DoesWindowParameterExist(std::string name)
	{
		auto item = windowVariables.map.find(name);
		if (item != windowVariables.map.end())
			return true; 
		return false;
	};


	int GetTotalParams()
	{
		return windowVariables.map.size(); 
	}
	 
	template<typename boolFunction> // any sdl function with 2 params: a window and a bool
	void SetParameter(std::string name, SDL_bool value, boolFunction func)
	{
		if (DoesWindowParameterExist(name))
		{
			func(window, value);
			windowVariables.map.at(name) = value;
		}
	}

	void SetFullscreen(Uint32 flags, SDL_bool value)
	{
		SDL_SetWindowFullscreen(window, flags); 
		windowVariables.map.at("FullDesktop") = (bool)value;
	}

public:
	//The window we'll be rendering to
	SDL_Window* window;

	//The surface contained by the window
	SDL_Surface* screen_surface;

	windowParams windowVariables;

private: 
	


};

#endif // __SmileWindow_H__