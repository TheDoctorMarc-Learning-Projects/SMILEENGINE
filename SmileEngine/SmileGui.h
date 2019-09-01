#pragma once

#include "SmileModule.h"

class SmileGui : public SmileModule
{
public:
	SmileGui(SmileApp* app, bool start_enabled = true);
	~SmileGui();
	bool Start(); // initialize after render and window creation
	update_status PreUpdate(float dt);  // call a new imgui frame and generate the gui
	void HandleRender(); 

	bool CleanUp();


private: 
	void GenerateGUI(); 
};
