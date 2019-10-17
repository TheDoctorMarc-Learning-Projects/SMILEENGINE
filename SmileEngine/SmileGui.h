#pragma once
#include "SmileModule.h"
#include <vector>

#define ACCESS_TO_GEOMETRY

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
	
	bool IsMouseOverTheGui() const; 
	bool IsGuiItemActive() const; 
	
private: 
	void FillMenuFunctionsVector(); 
	bool GenerateGUI(); 

private: 
	std::vector<void(*)(bool&)> menuFunctions;
	bool inMenu = false;

	friend class SmileSetup;
};

