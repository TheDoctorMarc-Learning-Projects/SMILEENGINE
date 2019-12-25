#define PAR_SHAPES_IMPLEMENTATION
#include "parshapes/par_shapes.h"

#include "SmileSetup.h"
#include "SmileApp.h"
#include "SmileScene.h"
#include "ComponentTransform.h"
/*#include <gl/GL.h>
//#include <gl/GLU.h>*/
#include"SmileSerialization.h"

#include "Glew/include/GL/glew.h"
#include "ComponentCamera.h"
#include "SmileGameObjectManager.h"
#include "SmileSpatialTree.h"

// Testing, remove later:
#include "Utility.h"
#include "SmileUtilitiesModule.h"
#include "RNG.h"

#ifdef NDEBUG //no debug
#pragma comment (lib, "MathGeoLib/libx86/ReleaseLib/MathGeoLib.lib") 
#else
#pragma comment (lib, "MathGeoLib/libx86/DebugLib/MathGeoLib.lib") 
#endif

#include "MathGeoLib/include/MathGeoLib.h"

#include "imgui/imgui.h"
#include "imgui/ImGuizmo.h"

#include "Resources_Components_Include.h"

SmileScene::SmileScene(SmileApp* app, bool start_enabled) : SmileModule(app, start_enabled)
{
}

SmileScene::~SmileScene()
{
	RELEASE(rootObj);
}

// Load assets
bool SmileScene::Start()
{ 
	// Root
	rootObj = DBG_NEW GameObject(DBG_NEW ComponentTransform(), "root");

	// Scene -> must already have cameras and it also created octree
    App->serialization->LoadScene("Library/Scenes/scene.json", true);

	// Test emitter here: 
	
 
	//GameObject* emitter = App->object_manager->CreateGameObject("Emitter", rootObj);
	//AllData data; 
	//data.initialState.life = std::pair(1.f, 0.2f);
	//data.emissionData.time = 0.1f;
	////data.emissionData.texPath = LIBRARY_TEXTURES_FOLDER_A + std::string("JapanFlag.dds"); 
	//data.emissionData.randomSpeed = std::pair(true, std::pair(float3(-2.f, 2.f, -2.f), float3(2.f, 2.f, 2.f)));
	//data.initialState.color.first = float4(1, 0, 0, 1); 
	//data.initialState.color.second = float4(0, 0, 1, 1); 
	//emitter->AddComponent((Component*)DBG_NEW ComponentParticleEmitter(emitter, data));
	return true;
}

bool SmileScene::CleanUp()
{
	rootObj->CleanUp();  
 
	selectedObj = nullptr; 
	selected_mesh = nullptr; 
	debugCamera = nullptr; 
	gameCamera = nullptr; 

	return true;
}

bool SmileScene::Reset() // similar, but root needs a transform after being cleaned, called by load scene
{
	rootObj->CleanUp();
	rootObj->AddComponent(DBG_NEW ComponentTransform()); 

	selectedObj = nullptr;
	selected_mesh = nullptr;
	debugCamera = nullptr;
	gameCamera = nullptr;

	return true;
}

void CreateRocketo()
{
	std::vector<Component*> comps;
	comps.push_back((Component*)DBG_NEW ComponentMesh(App->resources->Plane->GetUID(), "RocketoMesh"));
	comps.push_back((Component*)DBG_NEW ComponentVolatile(0.5f, &CreateFireWork, float3(1, 30, 0)));
	GameObject* rocketo = DBG_NEW GameObject(comps, "rocketo", App->scene_intro->rootObj);
	rocketo->Start();
	rocketo->SetStatic(false); 
	App->spatial_tree->OnStaticChange(rocketo, rocketo->GetStatic());
}


// Update
update_status SmileScene::Update(float dt)
{
	rootObj->Update(dt); 
	DrawObjects();
	//HandleGizmo();

	// TODO: firework with input 
	if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
		CreateRocketo(); 


	if (generalDbug == true)
	{
		DrawGrid();
		DebugLastRay();
	}

 
	return UPDATE_CONTINUE;
}


static void GetNonStaticRecursive(std::vector<GameObject*>& drawObjects, GameObject* obj)
{
	if (obj->GetStatic() == false)
		drawObjects.push_back(obj); 
	 
	for (auto& child : obj->childObjects)
		GetNonStaticRecursive(drawObjects, child); 

}

void SmileScene::DrawObjects()
{
	// 1) collect static candidates to be drawn: search for octree nodes inside frustrum 
	static std::vector<GameObject*> drawObjects;
	App->spatial_tree->CollectCandidatesA(drawObjects, App->renderer3D->targetCamera->calcFrustrum);

	// 2) add non-static ones 
	GetNonStaticRecursive(drawObjects, rootObj); 

	// (debug)
	objectCandidatesBeforeFrustrumPrune = drawObjects.size();

	// 3) then test all of their OBBs with the frustrum
	App->renderer3D->targetCamera->PruneInsideFrustrum(drawObjects);

	// (debug)
	objectCandidatesAfterFrustrumPrune = drawObjects.size();

	for (auto& obj : drawObjects)
		obj->Draw();

	drawObjects.clear(); 
}

update_status SmileScene::PostUpdate(float dt)
{
	rootObj->PostUpdate(); 
	return UPDATE_CONTINUE;
}
void SmileScene::HandleGizmo()
{
	if (selectedObj == nullptr)
		return; 

	static ImGuizmo::OPERATION op = ImGuizmo::OPERATION::TRANSLATE;
	if (App->input->GetKey(SDL_SCANCODE_E) == KEY_DOWN)
		op = ImGuizmo::OPERATION::ROTATE;
	if (App->input->GetKey(SDL_SCANCODE_R) == KEY_DOWN)
		op = ImGuizmo::OPERATION::SCALE;

	static ImGuizmo::MODE mode = ImGuizmo::MODE::WORLD; // TODOOO

	if (selectedObj != nullptr)
	{
		ImGuizmo::Enable(true);
	    
		float4x4 object = selectedObj->GetTransform()->GetGlobalMatrix().Transposed(); 
		mat4x4 view = debugCamera->GetViewMatrixTransposedA();
		mat4x4 proj = App->renderer3D->GetProjectionMatrixTransposedA(); 
		float4x4 diff;
		

		ImGuiIO& io = ImGui::GetIO();
		ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
		ImGuizmo::SetDrawlist();
		ImGuizmo::Manipulate(view.M, proj.M, op, mode, object.ptr(), diff.ptr());

		if (ImGuizmo::IsUsing() && !diff.IsIdentity())
			selectedObj->GetTransform()->SetLocalMatrix(object.Transposed());
	}
	else
		ImGuizmo::Enable(false);
}
		

void SmileScene::DrawGrid()
{
	glLineWidth(lineWidth);
	glColor3f(1, 1, 1);
	glBegin(GL_LINES);
	for (float i = 0; i < MAXLINES; i++)
	{
		if(i == 15)
			glColor3f(0.2f, 1.0f, 0.2f);
		else
			glColor3f(1, 1, 1);

		glVertex3f(i - MAXLINES * linesLength, 0, -MAXLINES * linesLength);
		glVertex3f(i - MAXLINES * linesLength, 0, MAXLINES * linesLength);
		glVertex3f(-MAXLINES * linesLength, 0, i - MAXLINES * linesLength);
		glVertex3f(MAXLINES * linesLength, 0, i - MAXLINES * linesLength);
	}
	glEnd();
	glColor3f(1, 1, 1);
	glLineWidth(lineWidth);

}

std::variant<ComponentMesh*, GameObject*> SmileScene::MouseOverMesh(int mouse_x, int mouse_y, bool assignClicked, bool GetMeshNotGameObject)
{

	// 0) Skip if general conditions are present (see the function)
	if (SkipRayConditions() == true)
	{
		assignClicked = false;
		return EmptyRayReturn(GetMeshNotGameObject, assignClicked);
	}

	// 1) Calculate the mouse pos in the world
	const float2 normalizedMousePos = GetNormalizedMousePos(mouse_x, mouse_y);

	// 2) Calculate a ray from the camera to the mouse pos in the world
	math::LineSegment ray = TraceRay(normalizedMousePos);

	// 3) Loop meshes in the screen and find an intersection with the ray
	ComponentMesh* found = FindRayIntersection(ray);

	// 4) Finally, return a mesh or a GameObject depending on the user's needs.
	// Only assign a selected mesh or Gameobject if requested. 
	if (assignClicked == false)
		return HoverRayReturn(GetMeshNotGameObject, found);
	else
		return ClickRayReturn(GetMeshNotGameObject, found);

}

bool SmileScene::SkipRayConditions() const
{
	if (App->input->GetKey(SDL_SCANCODE_LALT) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT
			|| App->gui->IsMouseOverTheGui())
			return true;

	return false;
}

void SmileScene::DebugLastRay()
{
	GLfloat p1[3] = { lastRay.a.x, lastRay.a.y, lastRay.a.z };
	GLfloat p2[3] = { lastRay.b.x, lastRay.b.y, lastRay.b.z };
	glColor3f(1, 0, 0);
	glBegin(GL_LINES);
	glVertex3fv(p1);
	glVertex3fv(p2);
	glEnd();
	glColor3f(1, 1, 1);
}

std::variant<ComponentMesh*, GameObject*> SmileScene::EmptyRayReturn(bool GetMeshNotGameObject, bool assignClicked)
{
	if (assignClicked)
	{
		selected_mesh = nullptr;
		selectedObj = nullptr;
		LOG("Unselected Mesh or Object in the scene :o");
	}

	if (GetMeshNotGameObject)
		return (ComponentMesh*)nullptr;
	else
		return (GameObject*)nullptr;
}


std::variant<ComponentMesh*, GameObject*> SmileScene::ClickRayReturn(bool GetMeshNotGameObject, ComponentMesh* found)
{
	if (found != nullptr)
	{
		// A) if nothing is selected, or another mesh / object is selected, select the mesh parent (object).
		// Do not select the parent if another mesh of the same parent is selected, in that case select the mesh 
		// Eg: One Click = select the object. Another clicks = select mesh
		if (
			(!selectedObj && !selected_mesh)
			|| (selectedObj && selectedObj != found->GetParent())
			|| (selected_mesh && selected_mesh->GetParent() != found->GetParent())
			)
		{

			selectedObj = found->GetParent();
			selected_mesh = nullptr;
			LOG("Selected GameObject in the scene :)");

			// Return nullptr if the user asked for a mesh, or the object is the user asked for an object
			if (GetMeshNotGameObject)
				return (ComponentMesh*)nullptr;
			else
				return (GameObject*)selectedObj;

		}

		else // B) if the object parent is selected, or another mesh of the parent, then select the mesh 
		{
			selected_mesh = found;
			selectedObj = nullptr;
			LOG("Selected Mesh in the scene :)");

			// Return nullptr if the user asked for a object, or the mesh if the user asked for a mesh
			if (GetMeshNotGameObject)
				return (ComponentMesh*)selected_mesh;
			else
				return (GameObject*)nullptr;

		}

	}
	else
		return EmptyRayReturn(GetMeshNotGameObject, true);
}


std::variant<ComponentMesh*, GameObject*> SmileScene::HoverRayReturn(bool GetMeshNotGameObject, ComponentMesh* found)
{
	if (GetMeshNotGameObject)
	{
		if (found)
			return found;
		else
			return (ComponentMesh*)nullptr;
	}

	// if the user asked for the parent gameObject
	else
	{
		if (found)
			return found->GetParent();
		else
			return (GameObject*)nullptr;
	}
}

ComponentMesh* SmileScene::FindRayIntersection(math::LineSegment ray)
{
	lastRay = ray;
	ComponentMesh* closest = nullptr; 
	float minDistance = (float)INT_MAX; 
	
	// See the function definition in the spatial tree header
	std::vector<GameObject*> objects; 
	App->spatial_tree->CollectCandidates(objects, ray); 

	for (auto& gameObject : objects)
	{
		// Get the mesh  
		ComponentMesh* mesh = dynamic_cast<ComponentMesh*>(gameObject->GetComponent(MESH));
		auto mesh_inf = mesh->GetResourceMesh()->GetMeshData(); 
		if (mesh_inf.index() == 1 || std::get<ModelMeshData*>(mesh_inf) == nullptr) // TODO: this skips own meshes (Plane etc) so either consider them or do not have them clickable (particle planes)
			continue;

		auto mesh_info = std::get<ModelMeshData*>(mesh_inf);
		// Find intersection then with mesh triangles
		for (int i = 0; i < mesh_info->num_vertex; i += 9) // 3 vertices * 3 coords (x,y,z) 
		{
			math::float3 v1(mesh_info->vertex[i], mesh_info->vertex[i + 1], mesh_info->vertex[i + 2]);
			math::float3 v2(mesh_info->vertex[i + 3], mesh_info->vertex[i + 4], mesh_info->vertex[i + 5]);
			math::float3 v3(mesh_info->vertex[i + 6], mesh_info->vertex[i + 7], mesh_info->vertex[i + 8]);

			// form a triangle and translate to global coords to test with ray!
			math::Triangle tri = math::Triangle(v1, v2, v3);
			math::float4x4 targetMat = gameObject->GetTransform()->GetGlobalMatrix(); 
			tri.Transform(targetMat); 

			// Finally check if the ray interesects with the face (triangle) 
			float newDistance = 0, intersectionPoint = 0; 
			if (lastRay.Intersects(tri, &newDistance, (math::float3*)nullptr))
			{
				// TODO: push check the closest, keep searching 
				// return dynamic_cast<ComponentMesh*>(mesh);
				if (newDistance < minDistance)
				{
					minDistance = newDistance; 
					closest = mesh;
				}
			
			}

		}
	}

	return closest;
}


float2 SmileScene::GetNormalizedMousePos(int mouse_x, int mouse_y)
{
	// 1) Translate from window coordinates to inverted Y in OpenGL 
	float windowW = (float)std::get<int>(App->window->GetWindowParameter("Width"));
	float windowH = (float)std::get<int>(App->window->GetWindowParameter("Height"));
	float2 mousePos((float)mouse_x, (float)mouse_y);

	// 2) Translate to normalized coords
	float2 normMousPos = mousePos;
	normMousPos.x = 2.F * mousePos.x / windowW - 1.F;
	normMousPos.y = 1.F - 2.F * (mousePos.y / windowH);  

	return normMousPos;
}

math::LineSegment SmileScene::TraceRay(float2 normMousePos)
{
	return App->renderer3D->targetCamera->calcFrustrum.UnProjectLineSegment(normMousePos.x, normMousePos.y);
}

void CreateFireWork()
{
	GameObject* emitter = App->object_manager->CreateGameObject("Emitter", App->scene_intro->rootObj);
	AllData data;
	data.initialState.life = std::pair(1.f, 0.2f);
	data.emissionData.time = 0.03f;
	data.emissionData.maxParticles = 1000; 
	data.emissionData.randomSpeed = std::pair(true, std::pair(float3(-2.f, 2.f, -2.f), float3(2.f, 2.f, 2.f)));
	
	// To test animated sheet (do with smoke): 
	data.emissionData.texPath = LIBRARY_TEXTURES_FOLDER_A + std::string("smokesheet.dds");
	data.initialState.tex = std::pair(true, 0.1f);

	// Color for fire (do not set alpha to 1, better show alpha blending)
	/*data.initialState.color.first = float4(1, 0, 0, 1);
	data.initialState.color.second = float4(0, 0, 1, 1);*/
	auto emmiterComp = DBG_NEW ComponentParticleEmitter(emitter, data);
	emitter->AddComponent((Component*)emmiterComp);

	emitter->GetTransform()->SetGlobalMatrix(App->scene_intro->rootObj->Find("rocketo")->GetTransform()->GetGlobalMatrix()); 

	
	emmiterComp->mesh->tileData = DBG_NEW TileData;
	emmiterComp->mesh->tileData->nCols = emmiterComp->mesh->tileData->nRows = 7;
	emmiterComp->mesh->tileData->maxTiles = 46;
}