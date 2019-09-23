#include "SmileSetup.h"
#include "SmileApp.h"
#include "SmileGui.h"


#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_opengl2.h"

#include <gl/GL.h>

#include <random>
#include "pcg/include/pcg_random.hpp"


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
	ImGui_ImplOpenGL2_Init();


	return ret;
}

// -----------------------------------------------------------------
update_status SmileGui::PreUpdate(float dt)
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
		ImGui_ImplSDL2_ProcessEvent(&event);

	// Start the Dear ImGui frame
	ImGui_ImplOpenGL2_NewFrame();
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

	 if(show_demo_window == true)
		 ImGui::ShowDemoWindow(&show_demo_window);
	{
		ImGui::Begin("Exit Window"); 
	
		if (ImGui::BeginMenu("Exit Menu"))
		{

			if (ImGui::MenuItem("Exit"))
				ret = false;
	
		 
			ImGui::EndMenu();
		}
		ImGui::End();

		ImGui::Begin("Default GUI Window");
		if (ImGui::BeginMenu("Default GUI Menu"))
		{

			if (ImGui::MenuItem("Demo Window"))
				show_demo_window = true;

			ImGui::EndMenu();
		}

		ImGui::End();
		
	}
 
	return ret; 
}

// -----------------------------------------------------------------
bool SmileGui::CleanUp()
{
	ImGui_ImplOpenGL2_Shutdown();
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
	ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

}