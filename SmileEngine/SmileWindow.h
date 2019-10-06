#ifndef __SmileWindow_H__
#define __SmileWindow_H__

#include "SmileModule.h"
#include "SDL/include/SDL.h"

#include <map>
#include <variant>
#include <string>

class SmileApp;

struct windowParams
{
	int Width, Height, Scale;
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

	std::variant<int, bool> GetWindowParameter(std::string name)
	{
		auto item = windowVariables.map.find(name);
		if (item != windowVariables.map.end())
			return windowVariables.map.at(name);
		return (int)NAN;
	}; 

	int GetTotalParams()
	{
		return windowVariables.map.size(); 
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