#pragma once
#include "SmileModule.h"
#include "ImGui/imgui.h"
#include <vector>

static void Configuration(bool& ret);
static void MainMenuBar(bool& ret); 
static void Console(bool& ret);

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

	ImGuiTextBuffer GetStartUpBuffer() { return startupLogBuffer; }; 
private: 
	void FillMenuFunctionsVector(); 


private: 
	bool GenerateGUI(); 

	std::vector<void(*)(bool&)> menuFunctions;
	ImGuiTextBuffer startupLogBuffer;
	
	friend class SmileSetup;
};

