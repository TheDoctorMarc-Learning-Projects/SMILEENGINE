#include "SmileSetup.h"
#include "SmileApp.h"
#include "SmileGui.h"
#include "SmileApp.h"
#include "SmileWindow.h"
#include "ResourceMesh.h"
#include "ResourceTexture.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/ImGuizmo.h"
#include <gl/GL.h>

#include <fstream>
#include "JSONParser.h"

#include "GameObject.h"
#include "Component.h"
#include "ComponentTypes.h"
#include "ComponentMesh.h"
#include "ComponentTransform.h"
#include "ComponentMaterial.h"

#include <filesystem>  
#include "SmileGameTimeManager.h"

// ----------------------------------------------------------------- [Minimal Containers to hold panel data: local to this .cpp]
namespace panelData
{
	bool configuration_view = false;
	bool console_view = false;

	namespace consoleSpace
	{
		ImGuiTextBuffer startupLogBuffer;
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

		namespace GeometryGeneratorGui
		{
			void Execute();
		}
	}

	namespace HierarchySpace
	{
		void Execute(bool& ret);
	}

	namespace InspectorSpace
	{
		void Execute(bool& ret);
		void ComponentData(Component*);
	}

	namespace PlaySpace
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
	menuFunctions.push_back(&panelData::HierarchySpace::Execute);
	menuFunctions.push_back(&panelData::InspectorSpace::Execute);
	menuFunctions.push_back(&panelData::PlaySpace::Execute);
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
	LOG("Initializing Imgui");

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

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

	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->window->window);
	ImGui::NewFrame();
	ImGuizmo::BeginFrame(); 

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

	LOG("Shutting down Imgui");

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	return true; 
}

// ----------------------------------------------------------------- called by Render cpp PostUpdate() 
void SmileGui::HandleRender()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

// ----------------------------------------------------------------- [Main Menu Bar]
void panelData::mainMenuSpace::Execute(bool& ret)
{
	if (TimeManager::IsPlaying())
		return;

	static bool showdemowindow = false;
	static bool showabout = false;

	if (ImGui::BeginMainMenuBar())
	{
		
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Quit"))
				ret = false;
			if (ImGui::MenuItem("Save Scene"))
				App->serialization->SaveScene(); 

			if (ImGui::MenuItem("Load Scene"))
				App->serialization->LoadScene("Library/Scenes/scene.json");

			ImGui::EndMenu();
		}

	
		GeometryGeneratorGui::Execute(); // CAUTION: this is a menu

		
		if (ImGui::BeginMenu("View"))
		{
			
			if (ImGui::MenuItem("Configuration")) 
				configuration_view = !configuration_view;
			
			if (ImGui::MenuItem("Console"))
				console_view = !console_view;
			

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Help"))
		{
			if (ImGui::MenuItem("Gui Demo")) {
				showdemowindow = !showdemowindow;
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
				showabout = !showabout;

			}
			if (ImGui::MenuItem("But can it run Crysis?"))
				App->input->ButCanItRunCrysis(); 


			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}
	if(showdemowindow)
		ImGui::ShowDemoWindow(&showdemowindow);

	if (showabout)
	{
		if (ImGui::Begin("About"), &showabout)
		{
			ImGui::TextWrapped("MIT License Copyright 2019 Marc Doctor and Eric Navarro");
			ImGui::NewLine();
			ImGui::TextWrapped("Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the 'Software'), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:");
			ImGui::NewLine();
			ImGui::TextWrapped("The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.");
			ImGui::NewLine();
			ImGui::TextWrapped("THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.");
		}
		ImGui::End();
	}
		
	
}

// ----------------------------------------------------------------- [Main Menu Bar: Geometry Generator GUI]
void panelData::mainMenuSpace::GeometryGeneratorGui::Execute()
{
	if (ImGui::BeginMenu("Geometry"))
	{
		// Primitive
		if (ImGui::MenuItem("Create Cube"))
		{
				// Create a mesh and an object
				ComponentMesh* mesh = DBG_NEW ComponentMesh(App->resources->Cube->GetUID(), "CubeMesh");
				GameObject* obj = App->object_manager->CreateGameObject(mesh, "CUBE", App->scene_intro->rootObj);
				obj->Start();

				// TODO: check this ok
				App->spatial_tree->OnStaticChange(obj, obj->GetStatic());
			
				
		}
		if (ImGui::MenuItem("Create Sphere"))
		{
			// Create a mesh and an object
			ComponentMesh* mesh = DBG_NEW ComponentMesh(App->resources->Sphere->GetUID(), "SphereMesh");
			GameObject* obj = App->object_manager->CreateGameObject(mesh, "SPHERE", App->scene_intro->rootObj);
			obj->Start();

			// TODO: check this ok
			App->spatial_tree->OnStaticChange(obj, obj->GetStatic());


		}
		
		ImGui::EndMenu(); 
	}
}

// ----------------------------------------------------------------- [Configuration]
void panelData::configSpace::Execute(bool& ret)
{
	static bool show_demo_window = false;
	if (configuration_view == true) {
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
			static bool cullFace = false, wireframe = false, depth = true, lightning = true,
				ColorMaterial = true, Texture2D = true, ambient = true, diffuse = true; 

			
		

			if (ImGui::Checkbox("Depth", &depth))
			{
				if (depth)
				{
					glEnable(GL_DEPTH_TEST);
					glDepthMask(GL_TRUE);
					glDepthFunc(GL_LEQUAL);
					glDepthRange(0.0f, 1.0f);
				}
				else
				{
					glDisable(GL_DEPTH_TEST);
					glDepthMask(GL_FALSE);
				}
				
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
			static bool windowcheckbox = false;
			static bool fullscreen_box = false;
			static bool resizable_box = false;
			static bool borderless_box = false;
			static bool fulldesktop_box = false;
			ImGui::Checkbox("Active", &windowcheckbox);

			ImGui::Text("Icon:");
			float br = 1.000;
			int width = App->window->windowVariables.Width;

			int height = App->window->windowVariables.Height;
			//int refresh_rate = ;
			//TODO path for the icon
			//Brightness
			if (ImGui::SliderFloat("Brightness", &br, 0.000, 1.000))
			{
				App->window->setBrightness(br);
			}
			//Width
			if (ImGui::SliderInt("Width", &width, 640, 1920))
			{
				App->window->SetWindowSize(width, height);
			}
			//Height
			if (ImGui::SliderInt("Height", &height, 480, 1080))
			{
				App->window->SetWindowSize(width, height);
			}


			//Refresh rate
			SDL_DisplayMode display_mode;
			int display_index = SDL_GetWindowDisplayIndex(App->window->window);
			SDL_GetDesktopDisplayMode(display_index, &display_mode);
			ImGui::Text("Refresh Rate:");
		

			// Resizable checkbox
			if (ImGui::Checkbox("Resizable", &resizable_box))
				App->window->SetParameter("Resizable", (SDL_bool)resizable_box, &SDL_SetWindowResizable);
		
			ImGui::SameLine();

			if (ImGui::Checkbox("Borderless", &borderless_box))
				App->window->SetParameter("Resizable", (SDL_bool)borderless_box, &SDL_SetWindowBordered); 
		
			ImGui::SameLine();

			if(ImGui::Checkbox("Full Desktop", &fulldesktop_box))
			{
			if(fulldesktop_box)
				App->window->SetFullscreen(SDL_WINDOW_FULLSCREEN_DESKTOP, (SDL_bool)fulldesktop_box);
			else
				App->window->SetFullscreen(0, (SDL_bool)fulldesktop_box);
			}
				

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
	
	ImGui::End();
}
	
}



void panelData::configSpace::CapsInformation() {

	if (TimeManager::IsPlaying())
		return; 

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


// ----------------------------------------------------------------- [Console]
void SmileGui::Log(const char* log)
{
	panelData::consoleSpace::startupLogBuffer.append(log); 
}

void panelData::consoleSpace::Execute(bool& ret)
{

	if (TimeManager::IsPlaying())
		return;

	static ImGuiTextFilter     Filter; 
	static bool consoleWindow; 
	static bool scrollToBottom = true; 
	if (console_view == true) {
		ImGui::Begin("Console", &consoleWindow);

		if (ImGui::Button("Clear"))
		{
			panelData::consoleSpace::startupLogBuffer.clear();
		}
		ImGui::SameLine();
		bool copy = ImGui::Button("Copy");
		
		ImGui::Separator();
		ImGui::BeginChild("scrolling");
		if (copy) ImGui::LogToClipboard();
		
		ImGui::TextUnformatted(panelData::consoleSpace::startupLogBuffer.begin());

		if (scrollToBottom)
		{
			ImGui::SetScrollHereY();
			scrollToBottom = false;
		}
		ImGui::EndChild();
		ImGui::End();
	}
	 
}


// ----------------------------------------------------------------- [Hierarchy]
static void ObjectRecursiveNode(GameObject* obj)
{

	if (App->scene_intro->debugCamera && obj && obj->GetCamera() && obj->GetCamera() == App->scene_intro->debugCamera)
		return; 

	if (obj)
	{
		if (ImGui::TreeNode(obj->GetName().c_str()))
		{
			 
			if (ImGui::IsItemClicked())
				App->scene_intro->selectedObj = obj; 

			if (obj->childObjects.size() > 0)  
			{
				for (auto& childObj : obj->childObjects)
					ObjectRecursiveNode(childObj);
			}
			
			ImGui::TreePop();
		}
		
	}

}

void panelData::HierarchySpace::Execute(bool& ret)
{

	if (TimeManager::IsPlaying())
		return;

	ImGui::SetNextWindowSize(ImVec2(250, 500));
	ImGui::SetNextWindowPos(ImVec2(20, 40));

	static bool showHierarchy = true; 

	ImGui::Begin("Hierarchy ", &showHierarchy); 
		
	for (auto& obj : App->scene_intro->rootObj->GetImmidiateChildren())
		ObjectRecursiveNode(obj);

	ImGui::End(); 
	
}

// ----------------------------------------------------------------- [Inspector]
void panelData::InspectorSpace::Execute(bool& ret)
{

	if (TimeManager::IsPlaying())
		return;


	ImGui::SetNextWindowSize(ImVec2(400, 500));
	ImGui::SetNextWindowPos(ImVec2(870, 250));


	static bool showInspector = true;
	static const ImVec4 c(11, 100, 88, 255); 

	ImGui::Begin("Inspector Panel", &showInspector); 
	
		GameObject* selected = App->scene_intro->selectedObj;
		if (selected != nullptr)
		{
			// General info
			ImGui::TextColored(c, selected->GetName().c_str());
			static bool isStatic = selected->GetStatic(); 
			if (ImGui::Checkbox("Static", &isStatic))
				selected->SetStatic(isStatic); 

			// Guizmo: it gets the transform values, and also updates the transform if changed
			selected->ShowTransformInspector(); 

			// Loop the object's components
			std::array<Component*, MAX_COMPONENT_TYPES> components = selected->GetComponents(); 

			for(auto& c : components)
				if (c && c->GetComponentType() != TRANSFORM)
					panelData::InspectorSpace::ComponentData(c);


			// Other stuff
			if (ImGui::TreeNode("Bounding Boxes"))
			{
				auto GetStringFrom3Values = [](float3 xyz, bool append) -> std::string
				{
					return std::string(
						std::string((append) ? "(" : "") + std::to_string(xyz.x)
						+ std::string((append) ? ", " : "") + std::to_string(xyz.y)
						+ std::string((append) ? ", " : "") + std::to_string(xyz.z))
						+ std::string((append) ? ")" : "");
				};

				if (ImGui::TreeNode("AABB"))
				{

					math::AABB AABB = selected->GetBoundingData().AABB;
					ImGui::Text(std::string("Global Center:" + GetStringFrom3Values(AABB.CenterPoint(), true)).c_str());
					ImGui::Text(std::string("Volume:" + std::to_string(AABB.Volume())).c_str());
					ImGui::Checkbox("Show AABB", &selected->debugData.AABB);
					ImGui::TreePop();
				}

				if (ImGui::TreeNode("OBB"))
				{
					math::OBB OBB = selected->GetBoundingData().OBB;
					ImGui::Text(std::string("Global Center:" + GetStringFrom3Values(OBB.CenterPoint(), true)).c_str());
					ImGui::Text(std::string("Volume:" + std::to_string(OBB.Volume())).c_str());
					ImGui::Checkbox("Show OBB", &selected->debugData.OBB);
					ImGui::TreePop();
				}

			
				ImGui::TreePop();
			}

			// Deletion
			if (ImGui::Button("Delete"))
			{
				App->object_manager->DestroyObject(App->scene_intro->selectedObj);
				App->scene_intro->selectedObj = nullptr; 
			}
		


		}
	
		ImGui::End(); 
}

void panelData::InspectorSpace::ComponentData(Component* c)
{
	KEY_STATE keyState = App->input->GetKey(SDL_SCANCODE_KP_ENTER);

	auto GetStringFrom3Values = [](float3 xyz, bool append) -> std::string
	{
		return std::string(
			std::string((append) ? "(" : "") + std::to_string(xyz.x)
			+ std::string((append) ? ", " : "") + std::to_string(xyz.y)
			+ std::string((append) ? ", " : "") + std::to_string(xyz.z))
			+ std::string((append) ? ")" : "");
	};


	if (ImGui::TreeNode(c->GetName().c_str()))
	{
		switch (c->GetComponentType())
		{
		case COMPONENT_TYPE::MATERIAL:
		{
			ComponentMaterial* mat = dynamic_cast<ComponentMaterial*>(c);
			textureData* data = mat->GetTextureData(); 

			// TODO: show my linked resource's reference count
			ImGui::Text(std::string("Attached resource reference count: " + std::to_string(mat->GetResourceTexture()->GetReferenceCount())).c_str());
			ImGui::Text(std::string("Path: " + mat->GetTextureData()->path).c_str());
			ImGui::Text(std::string("Size: " + std::to_string(mat->GetTextureData()->width) + " x " + std::to_string(mat->GetTextureData()->height)).c_str());
			if (ImGui::Button("Change Texture")) // TODO: filesystem (muahahahaha)
			{
				const std::filesystem::path& relativePath = "Assets/";
				std::filesystem::path& absolutePath = std::filesystem::canonical(relativePath);
				ShellExecute(NULL, "open", absolutePath.string().c_str(), NULL, NULL, SW_SHOWDEFAULT);
			}
			if (ImGui::Button("Change Texture to checkers")) 
				App->fbx->AssignCheckersTextureToObj(mat->GetParent());
	
			ImGui::Image((ImTextureID)data->id_texture, ImVec2(data->width, data->height)); 

			break;
		}

		case COMPONENT_TYPE::MESH:
		{
			ComponentMesh* mesh = dynamic_cast<ComponentMesh*>(c);
			ImGui::Text(std::string("Attached resource reference count: " + std::to_string(mesh->GetResourceMesh()->GetReferenceCount())).c_str());
			ImGui::Text(std::string("Number of vertices: " + std::to_string(mesh->GetResourceMesh()->model_mesh->num_vertex)).c_str());
			ImGui::Text(std::string("Bounding sphere radius: " + std::to_string(mesh->GetParent()->GetBoundingSphereRadius())).c_str());
			std::string type = ((mesh->GetMeshType() == MODEL) ? "Model" : "Primitive");
			ImGui::Text(std::string("Type: " + type).c_str());

			// debug
			ImGui::Checkbox("Show vertex normals", &mesh->debugData.vertexNormals); 
			ImGui::Checkbox("Show face normals", &mesh->debugData.faceNormals);
			break;
		}

		case COMPONENT_TYPE::CAMERA:
		{
			ComponentCamera* cam = dynamic_cast<ComponentCamera*>(c);
			renderingData camData = cam->GetRenderingData(); 
			float fovY[1], pNearDist[1], pFarDist[1]; 
			fovY[0] = camData.fovYangle;
			pNearDist[0] = camData.pNearDist;
			pFarDist[0] = camData.pFarDist;
			ImGui::InputFloat("Field Of View Y", fovY);  
			ImGui::InputFloat("Distance to near plane", pNearDist); 
			ImGui::InputFloat("Distance to far plane", pFarDist); 

			if (keyState == KEY_DOWN)
				cam->OnInspector(fovY, pNearDist, pFarDist);

				
			break; 
		}

		default:
		{
			break;
		}

			
		}


		ImGui::TreePop();
	}
	
}


// ----------------------------------------------------------------- [Play]
void panelData::PlaySpace::Execute(bool& ret)
{
 
	std::string playStop = (TimeManager::IsPlaying()) ? "Stop" : "Play"; 
	std::string pauseResume = (TimeManager::IsPlaying()) ? "Pause" : "Resume";


	ImGui::SetNextWindowSize(ImVec2(330, 22));
	ImGui::SetNextWindowPos(ImVec2(500, 30));

	// Do stuff
	ImGui::Begin("Game", 0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize 
		| ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar);
	if (ImGui::Button(playStop.c_str(), ImVec2(100, 20)))
		TimeManager::PlayButton();
	ImGui::SameLine(); 
	if (ImGui::Button(pauseResume.c_str(), ImVec2(100, 20)))
		TimeManager::PauseButton();
	ImGui::SameLine();
	if (ImGui::Button("|> ||", ImVec2(100, 20)))
		TimeManager::PlayOne();
	ImGui::End();

	ImGui::SetNextWindowSize(ImVec2(400, 150));
	ImGui::SetNextWindowPos(ImVec2(870, 25));


	// Show stuff
	ImGui::Begin("Sesion", 0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize
		| ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar);

	if (playStop == "Stop")
	{
		ImGui::Text("Game Speed Multiplier: ");
		ImGui::SameLine();
		float multi = TimeManager::_timeData.gameTimeScale;
		if (ImGui::SliderFloat("Times", &multi, 1, 10))
			App->SetDtMultiplier(TimeManager::_timeData.gameTimeScale = multi);
	}


	ImGui::Text("Total Seconds Since Startup: ");
	ImGui::SameLine();
	ImGui::Text(std::to_string(TimeManager::realTimeClock.ReadSec()).c_str());

	ImGui::Text("Total Seconds Since Game Start: ");
	ImGui::SameLine();
	ImGui::Text(std::to_string(TimeManager::gameClock.ReadSec()).c_str());

	ImGui::Text("App Delta Time: ");
	ImGui::SameLine();
	ImGui::Text(std::to_string(App->GetDtNoMulti()).c_str());

	ImGui::Text("Game Delta Time: ");
	ImGui::SameLine();
	ImGui::Text((playStop == "Stop") ? std::to_string(App->GetDT() * App->GetDTMulti()).c_str() : "Not playing");

	ImGui::Text("Frames since Startup: ");
	ImGui::SameLine();
	ImGui::Text(std::to_string(App->GetFrameCount()).c_str());

	ImGui::End();
}

// ----------------------------------------------------------------- (Utilities)

bool SmileGui::IsMouseOverTheGui() const
{
	return ImGui::IsAnyItemHovered();
}

bool SmileGui::IsGuiItemActive() const
{
	return ImGui::IsAnyItemActive();
}