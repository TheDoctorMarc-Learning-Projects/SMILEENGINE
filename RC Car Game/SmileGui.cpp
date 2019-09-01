#include "Globals.h"
#include "Application.h"
#include "SmileGui.h"


#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_opengl2.h"

SmileGui::SmileGui(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

// Destructor
SmileGui::~SmileGui()
{
}

// this is called after Render and Window Init(), so the window and the render context are available 
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


// PostUpdate present buffer to screen
update_status SmileGui::Update(float dt)
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
		ImGui_ImplSDL2_ProcessEvent(&event);

	// Start the Dear ImGui frame
	ImGui_ImplOpenGL2_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->window->window);
	ImGui::NewFrame();

	// create the gui elements
	GenerateGUI(); 

	return UPDATE_CONTINUE;
}

void SmileGui::GenerateGUI()
{
	bool show_demo_window = true; 
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);


	if (show_demo_window)
		ImGui::ShowDemoWindow(&show_demo_window);

	// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
	{
		static float f = 0.0f;
		static int counter = 0;

		ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

		ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
		ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
		
		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
		ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

		if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
			counter++;
		ImGui::SameLine();
		ImGui::Text("counter = %d", counter);

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
	}
 

	//ImGui::Render();
}


// PostUpdate present buffer to screen
update_status SmileGui::PostUpdate(float dt)
{
	
	return UPDATE_CONTINUE;
}


// Called before quitting
bool SmileGui::CleanUp()
{
	ImGui_ImplOpenGL2_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	return true; 
}