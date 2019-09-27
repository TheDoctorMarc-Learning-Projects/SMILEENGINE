#include "SmileSetup.h"
#include "SmileApp.h"
#include "SmileGui.h"
#include "SmileApp.h"
#include "SmileWindow.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_opengl3.h"

#include <gl/GL.h>



SmileGui::SmileGui(SmileApp* app, bool start_enabled) : SmileModule(app, start_enabled)
{
}

SmileGui::~SmileGui()
{
}

// -----------------------------------------------------------------
bool SmileGui::Start()
{
	bool ret = true; 

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	// Setup Platform/Renderer bindings
	ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->renderer3D->context);
	ImGui_ImplOpenGL3_Init();


	return ret;
}

// -----------------------------------------------------------------
update_status SmileGui::PreUpdate(float dt)
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
		ImGui_ImplSDL2_ProcessEvent(&event);

	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->window->window);
	ImGui::NewFrame();

	// create the gui elements
	if (GenerateGUI() == false)
		return UPDATE_STOP; 
 

	return UPDATE_CONTINUE;
}

// -----------------------------------------------------------------
bool SmileGui::GenerateGUI()
{
	bool ret = true; 
	static bool show_demo_window = false;
	bool windowcheckbox = false;

	 

		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Quit"))
					ret = false;
				ImGui::EndMenu();
			}
			
			ImGui::EndMainMenuBar();
		}

		if (ImGui::Begin("Configuration")) {
			ImGuiIO& io = ImGui::GetIO();
			if (ImGui::BeginMenu("Options")) {
				ImGui::MenuItem("Set Defaults");

					ImGui::MenuItem("Load");

					ImGui::MenuItem("Load");

					ImGui::EndMenu();
			}
			if (ImGui::CollapsingHeader("Application")) {
				static char str0[128] = "Smile Engine";
				ImGui::InputText("App Name", str0, IM_ARRAYSIZE(str0));
				App->window->SetTitle(str0);
				static char str1[128] = "UPC CITM";
				ImGui::InputText("Organitzation", str1, IM_ARRAYSIZE(str1));
				static int i1 = 0;
				ImGui::SliderInt("Max FPS", &i1, 0, 120);
				ImGui::Text("Limit Framerate: %i", i1);
				
				char title[25];
				sprintf_s(title, 25, "Framerate %.1f", App->fps_log[App->fps_log.size() - 1]);
				ImGui::PlotHistogram("##framerate", &App->fps_log[0], App->fps_log.size(), 0, title, 0.0f, 100.0f, ImVec2(310, 100));
				sprintf_s(title, 25, "Milliseconds %.1f", App->ms_log[App->ms_log.size() - 1]);
				ImGui::PlotHistogram("##milliseconds", &App->ms_log[0], App->ms_log.size(), 0, title, 0.0f, 40.0f, ImVec2(310, 100));			
				
			}
			if (ImGui::CollapsingHeader("Window")) {
				ImGui::Checkbox("Active", &windowcheckbox);
			}
			if (ImGui::CollapsingHeader("File System")) {
				
			}
			if (ImGui::CollapsingHeader("Input")) {
				
			}
			if (ImGui::CollapsingHeader("Hardware")) {
				
			}
		
		
		ImGui::End();
		
	}
 
	return ret; 
}

// -----------------------------------------------------------------
bool SmileGui::CleanUp()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	return true; 
}

// ----------------------------------------------------------------- called by Render cpp PostUpdate() 
void SmileGui::HandleRender()
{
	//ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	// Rendering
	ImGui::Render();
	glViewport(0, 0, (int)ImGui::GetIO().DisplaySize.x, (int)ImGui::GetIO().DisplaySize.y);
	//glUseProgram(0); // You may want this if using this code in an OpenGL 3+ context where shaders may be bound
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

}