#ifndef __SmileWindow_H__
#define __SmileWindow_H__

#include "SmileModule.h"
#include "SDL/include/SDL.h"

class SmileApp;

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
	//The window we'll be rendering to
	SDL_Window* window;

	//The surface contained by the window
	SDL_Surface* screen_surface;
};

#endif // __SmileWindow_H__