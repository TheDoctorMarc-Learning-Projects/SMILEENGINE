#pragma once
#include "SmileModule.h"
#include "ImGui/imgui.h"
#include <vector>

class SmileGui : public SmileModule
{
public:
	SmileGui(SmileApp* app, bool start_enabled = true);
	~SmileGui();
	bool Start(); // initialize after render and window creation
	update_status PreUpdate(float dt);  // call a new imgui frame and generate the gui
	void HandleRender(); 
	void Log(const char* log);
	bool CleanUp();
	
private: 
	void FillMenuFunctionsVector(); 
	bool GenerateGUI(); 

private: 
	std::vector<void(*)(bool&)> menuFunctions;
	
	friend class SmileSetup;
};

