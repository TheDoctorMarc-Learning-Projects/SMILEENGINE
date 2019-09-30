#include "SmileSetup.h"
#include "SmileApp.h"
#include "SmileGui.h"
#include "SmileApp.h"
#include "SmileWindow.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_opengl3.h"

#include <gl/GL.h>

// ----------------------------------------------------------------- [Minimal Containers to hold panel data: local to this .cpp]
namespace panelData
{
	namespace consoleSpace
	{
		static ImGuiTextBuffer startupLogBuffer;
		void Execute(bool& ret);
		void ShutDown() { startupLogBuffer.clear(); };
	}
	namespace configSpace
	{
		void Execute(bool& ret);
	}
	namespace mainMenuSpace
	{
		void Execute(bool& ret);
	}

}

// -----------------------------------------------------------------
SmileGui::SmileGui(SmileApp* app, bool start_enabled) : SmileModule(app, start_enabled)
{
	FillMenuFunctionsVector();
}

// -----------------------------------------------------------------
void SmileGui::FillMenuFunctionsVector()
{
	menuFunctions.push_back(&panelData::consoleSpace::Execute);
	menuFunctions.push_back(&panelData::configSpace::Execute);
	menuFunctions.push_back(&panelData::mainMenuSpace::Execute);
}

// -----------------------------------------------------------------
SmileGui::~SmileGui()
{
	menuFunctions.clear(); 
	panelData::consoleSpace::ShutDown(); 
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

	for (auto& func : menuFunctions)
		func(ret); 
 
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

// ----------------------------------------------------------------- [Main Menu Bar]
void panelData::mainMenuSpace::Execute(bool& ret)
{
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
}

// ----------------------------------------------------------------- [Configuration]
void panelData::configSpace::Execute(bool& ret)
{
	static bool show_demo_window = false;
	bool windowcheckbox = false;

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
		if (ImGui::CollapsingHeader("Rendering")) {
			static bool depth, cullFace, lightning, ColorMaterial, Texture2D, ambient, diffuse, wireframe = false;
			if (ImGui::Checkbox("Depth", &depth))
			{
				if (depth)
					glEnable(GL_DEPTH_TEST);
				else
					glDisable(GL_DEPTH_TEST);

			}

			if (ImGui::Checkbox("Cull face", &cullFace))
			{
				if (cullFace)
					glEnable(GL_CULL_FACE);
				else
					glDisable(GL_CULL_FACE);
			}

			if (ImGui::Checkbox("Lightning", &lightning))
			{
				if (lightning)
					glEnable(GL_LIGHTING);
				else
					glDisable(GL_LIGHTING);
			}


			if (ImGui::Checkbox("Color material", &ColorMaterial))
			{
				if (ColorMaterial)
					glEnable(GL_COLOR_MATERIAL);
				else
					glDisable(GL_COLOR_MATERIAL);
			}

			if (ImGui::Checkbox("Texture 2D", &Texture2D))
			{
				if (Texture2D)
					glEnable(GL_TEXTURE_2D);
				else
					glDisable(GL_TEXTURE_2D);
			}

			if (ImGui::Checkbox("Ambient", &ambient))
			{
				if (ambient)
					glEnable(GL_AMBIENT);
				else
					glDisable(GL_AMBIENT);
			}


			if (ImGui::Checkbox("Diffuse", &diffuse))
			{
				if (diffuse)
					glEnable(GL_DIFFUSE);
				else
					glDisable(GL_DIFFUSE);
			}


			if (ImGui::Checkbox("Wireframe", &wireframe))
			{
				if (wireframe)
					glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				else
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			}



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
}


// ----------------------------------------------------------------- [Console]
void SmileGui::Log(const char* log)
{
	panelData::consoleSpace::startupLogBuffer.append(log); 
}

void panelData::consoleSpace::Execute(bool& ret)
{
	static ImGuiTextFilter     Filter; 
	static bool consoleWindow; 
	static bool scrollToBottom = true; 
	
	ImGui::Begin("Console", &consoleWindow);

	if (ImGui::Button("Clear"))
	{
		panelData::consoleSpace::startupLogBuffer.clear();
	}
	ImGui::SameLine();
	bool copy = ImGui::Button("Copy");
	ImGui::SameLine();
	Filter.Draw("Filter", -100.0f);
	ImGui::Separator();
	ImGui::BeginChild("scrolling");
	//ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 1));
	if (copy) ImGui::LogToClipboard();


	//if (Filter.IsActive())
	//{
	//	const char* buf_begin = Buf.begin();
	//	const char* line = buf_begin;
	//	/*for (int line_no = 0; line != NULL; line_no++)
	//	{
	//		const char* line_end = (line_no < LineOffsets.Size) ? buf_begin + LineOffsets[line_no] : NULL;
	//		if (Filter.PassFilter(line, line_end))
	//			ImGui::TextUnformatted(line, line_end);
	//		line = line_end && line_end[1] ? line_end + 1 : NULL;
	//	}*/
	//}
	//else
	//{
	ImGui::TextUnformatted(panelData::consoleSpace::startupLogBuffer.begin());
	//}

	if (scrollToBottom)
	{
		ImGui::SetScrollHereY(); 
		scrollToBottom = false;
	}
		

	//ImGui::PopStyleVar();
	ImGui::EndChild();
	ImGui::End();
	 
}