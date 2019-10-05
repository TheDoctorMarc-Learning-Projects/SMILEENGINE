#include "SmileSetup.h"
#include "SmileApp.h"
#include "SmileGui.h"
#include "SmileApp.h"
#include "SmileWindow.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_opengl3.h"

#include <gl/GL.h>

#include <fstream>
#include "JSONParser.h"

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
		void CapsInformation();
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
		bool showdemowindow = true;
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Quit"))
				ret = false;
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("View"))
		{
			if (ImGui::MenuItem("Configuration")) {
				//Todo show/hide config panel
				
			}
			
			if (ImGui::MenuItem("Console")) {
				//Todo show/hide console panel
			}

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Help"))
		{
			if (ImGui::MenuItem("Gui Demo")) {
				showdemowindow = true;
				if(showdemowindow)
					ImGui::ShowDemoWindow(&showdemowindow);

				//TODO The demo window doesnt appears

			}
			if (ImGui::MenuItem("Documentation")) {
				ShellExecuteA(NULL, "open", "https://github.com/thedoctormarc/SMILEENGINE", NULL, NULL, SW_SHOWNORMAL);
				//Change url to the wiki
			
			}
			if (ImGui::MenuItem("Download Latest")) {
				ShellExecuteA(NULL, "open", "https://github.com/thedoctormarc/SMILEENGINE/releases", NULL, NULL, SW_SHOWNORMAL);

			}
			if (ImGui::MenuItem("Report a bug")) {
				ShellExecuteA(NULL, "open", "https://github.com/thedoctormarc/SMILEENGINE/issues", NULL, NULL, SW_SHOWNORMAL);
			}
			if (ImGui::MenuItem("About")) {
				//TODO

			}

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}
}

// ----------------------------------------------------------------- [Configuration]
void panelData::configSpace::Execute(bool& ret)
{
	static bool show_demo_window = false;
	
	ImGui::Begin("Configuration");
	ImGuiIO& io = ImGui::GetIO();
		if (ImGui::BeginMenu("Options")) {
			ImGui::MenuItem("Set Defaults");

			ImGui::MenuItem("Load");

			
			if (ImGui::MenuItem("Save"))
			{
				std::ofstream saveConfigFile("config.json"); 
				rapidjson::StringBuffer buffer;
				rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
				
 
				// window 
				writer.StartObject();
				writer.Key("Window");

				writer.StartArray(); 

				writer.StartObject();
				
				writer.Key("Width");
				writer.Int(std::get<int>(App->window->GetWindowParameter("Width")));
			
				writer.Key("Height");
				writer.Int(std::get<int>(App->window->GetWindowParameter("Height")));
	
				writer.Key("Scale");
				writer.Int(std::get<int>(App->window->GetWindowParameter("Scale")));

				writer.Key("Fullscreen");
				writer.Bool(std::get<bool>(App->window->GetWindowParameter("Fullscreen")));
		
				writer.Key("Borderless");
				writer.Bool(std::get<bool>(App->window->GetWindowParameter("Borderless")));

				writer.Key("Resizable");
				writer.Bool(std::get<bool>(App->window->GetWindowParameter("Resizable")));
			
				writer.Key("FullDesktop");
				writer.Bool(std::get<bool>(App->window->GetWindowParameter("FullDesktop")));

				writer.EndObject();

				writer.EndArray();

				writer.EndObject(); 

				const char* output = buffer.GetString();
				std::string strOutput(output); 
				saveConfigFile << output;
				saveConfigFile.close(); 
			}

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
			bool windowcheckbox = false;
			bool fullscreen_box = false;
			bool resizable_box = false;
			bool borderless_box = false;
			bool fulldesktop_box = false;
			ImGui::Checkbox("Active", &windowcheckbox);
			
			ImGui::Text("Icon:");
			float br = 1.000;
			int width = App->window->windowVariables.Width;
			
			int height = App->window->windowVariables.Height;
			//int refresh_rate = ;
			//TODO path for the icon
			//Brightness
			if(ImGui::SliderFloat("Brightness", &br, 0.000, 1.000))
				SDL_SetWindowBrightness(App->window->window, br);
			//Width
			if (ImGui::SliderInt("Width", &width, 640, 1920))
			{
				SDL_SetWindowSize(App->window->window, width, height);
				App->renderer3D->OnResize(width, height);
			}
			//Height
			if (ImGui::SliderInt("Height", &height, 480, 1080))
			{
				SDL_SetWindowSize(App->window->window, width, height);
				App->renderer3D->OnResize(width, height);
			}
			
			
			//Refresh rate
			SDL_DisplayMode display_mode;
			int display_index = SDL_GetWindowDisplayIndex(App->window->window);
			SDL_GetDesktopDisplayMode(display_index, &display_mode);
			ImGui::Text("Refresh Rate:");
			ImGui::SameLine();
			ImGui::TextColored({ 255,255,0,255 }, "%i", display_mode.refresh_rate);
			//Fullscreen checkbox
			ImGui::Checkbox("FullScreen", &fullscreen_box);
			ImGui::SameLine();
			if (fullscreen_box)
			{
				SDL_SetWindowFullscreen(App->window->window, SDL_WINDOW_FULLSCREEN);
			}
			else 
			{
				SDL_SetWindowFullscreen(App->window->window, 0);
			}
			// Resizable checkbox
			ImGui::Checkbox("Resizable", &resizable_box);
			
			if (resizable_box)
			{
				SDL_SetWindowResizable(App->window->window, (SDL_bool)false);
			}
			else
			{
				SDL_SetWindowResizable(App->window->window, (SDL_bool)true);
			}
			ImGui::Checkbox("Borderless", &borderless_box);
			if (borderless_box)
			{
				SDL_SetWindowBordered(App->window->window, (SDL_bool)false);
			}
			else {
				SDL_SetWindowBordered(App->window->window, (SDL_bool)true);
			}
			ImGui::SameLine();
			ImGui::Checkbox("Full Desktop", &fulldesktop_box);
			if (fulldesktop_box)
			{
				SDL_SetWindowFullscreen(App->window->window, SDL_WINDOW_FULLSCREEN_DESKTOP);
			}
			else
			{
				SDL_SetWindowFullscreen(App->window->window, 0);
			}
		}
		if (ImGui::CollapsingHeader("File System")) {
			ImGui::Text("Base Path:");

			ImGui::Text("Read Path:");

			ImGui::Text("Write Path:");

		}
		if (ImGui::CollapsingHeader("Input")) {
			bool inputcheckbox = true;
			ImGui::Checkbox("Active", &inputcheckbox);
			ImGui::Text("Mouse Position: ");
			ImGui::SameLine();
			ImGui::TextColored({ 255,255,0,255 }, "%i,%i", App->input->GetMouseX(), App->input->GetMouseY());
			ImGui::Text("Mouse Motion: ");
			ImGui::SameLine();
			ImGui::TextColored({ 255,255,0,255 }, "%i,%i", App->input->GetMouseXMotion(), App->input->GetMouseYMotion());
			ImGui::Text("Mouse Wheel: ");
			ImGui::SameLine();
			ImGui::TextColored({ 255,255,0,255 }, "%i", App->input->GetMouseZ());
		}
		if (ImGui::CollapsingHeader("Hardware")) {
			bool hardwarecheckbox = false;
			int core = SDL_GetCPUCount();
			int cache_size = SDL_GetCPUCacheLineSize();
			float ram = SDL_GetSystemRAM();
			ImGui::Checkbox("Active", &hardwarecheckbox);
			ImGui::Text("SDL Version:");
			ImGui::Text("CPUs:");
			ImGui::SameLine();
			ImGui::TextColored({ 255,255,0,255 }, "%i", core);
			ImGui::Text("System RAM:");
			ImGui::SameLine();
			ImGui::TextColored({ 255,255,0,255 }, "%.1f Gb", ram/1000);
			CapsInformation();
			
		}

		ImGui::End();
		
	
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

void panelData::configSpace::CapsInformation() {
	
	bool rdtsc = SDL_HasRDTSC();
	bool mmx = SDL_HasMMX();
	bool sse = SDL_HasSSE();
	bool sse2 = SDL_HasSSE2();
	bool sse3 = SDL_HasSSE3();
	bool sse41 = SDL_HasSSE41();
	bool sse42 = SDL_HasSSE42();
	bool avx = SDL_HasAVX();
	bool avx2 = SDL_HasAVX2();


	ImGui::Text("Caps:");
	ImGui::SameLine();
	if (rdtsc)
		ImGui::TextColored({ 255,255,0,255 }, "RDTSC,");
	ImGui::SameLine();
	if (mmx)
		ImGui::TextColored({ 255,255,0,255 }, "MMX,");
	ImGui::SameLine();
	if (sse)
		ImGui::TextColored({ 255,255,0,255 }, "SSE,");
	ImGui::SameLine();
	if (sse2)
		ImGui::TextColored({ 255,255,0,255 }, "SSE2,");
	ImGui::SameLine();
	if (sse3)
		ImGui::TextColored({ 255,255,0,255 }, "SSE3,");
	ImGui::SameLine();
	if (sse41)
		ImGui::TextColored({ 255,255,0,255 }, "SSE41,");
	ImGui::SameLine();
	if (sse42)
		ImGui::TextColored({ 255,255,0,255 }, "SSE42,");
	ImGui::SameLine();
	if (avx)
		ImGui::TextColored({ 255,255,0,255 }, "AVX,");
	ImGui::SameLine();
	if (avx2)
		ImGui::TextColored({ 255,255,0,255 }, "AVX2");
	const char* gpu = (const char*)glGetString(GL_VENDOR);
	ImGui::Text("GPU:");
	ImGui::SameLine();
	ImGui::TextColored({ 255,255,0,255 }, gpu);

	const char* brand = (const char*)glGetString(GL_RENDERER);
	ImGui::Text("Brand:");
	ImGui::SameLine();
	ImGui::TextColored({ 255,255,0,255 }, brand);

	/*float vram_budget = ;
	float vram_usage = ;
	float vram_available = ;
	float vram_reserved = ;*/

	ImGui::Text("VRAM Budget:");
	//ImGui::SameLine();
	//ImGui::TextColored({ 255,255,0,255 }, vram_budget);
	ImGui::Text("VRAM Usage:");
	//ImGui::SameLine();
	//ImGui::TextColored({ 255,255,0,255 }, vram_usage);
	ImGui::Text("VRAM Available:");
	//ImGui::SameLine();
	//ImGui::TextColored({ 255,255,0,255 }, vram_available);
	ImGui::Text("VRAM Reserved:");
	//ImGui::SameLine();
	//ImGui::TextColored({ 255,255,0,255 }, vram_reserved);
}