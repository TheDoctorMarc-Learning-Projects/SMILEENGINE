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

	// Just testing Spatial Tree
	/*float size[10]; 
	for (auto& i : size)
	{
		float3 pos(0, 0, 0); 
		pos.x = std::get<float>(dynamic_cast<RNG*>(App->utilities->GetUtility("RNG"))->GetRandomValue(-10.F, 10.F)); 
		pos.y = std::get<float>(dynamic_cast<RNG*>(App->utilities->GetUtility("RNG"))->GetRandomValue(0.F, 20.F));
		pos.z = std::get<float>(dynamic_cast<RNG*>(App->utilities->GetUtility("RNG"))->GetRandomValue(-10.F, 10.F));
		GameObject* daHouse = App->fbx->LoadFBX("Assets/Models/BakerHouse.fbx"); 
		daHouse->GetTransform()->ChangePosition(pos);

	}*/
		
	// Debug Camera
	GameObject* debugCameraObj = DBG_NEW GameObject(DBG_NEW ComponentTransform(float3(0, 0, 30)), "Debug Camera", rootObj);
	debugCamera = DBG_NEW ComponentCamera(debugCameraObj, vec3(0, 0, -1)); 
	debugCameraObj->AddComponent(debugCamera);

	// Game Camera
	GameObject* gameCameraObj = DBG_NEW GameObject(DBG_NEW ComponentTransform(float3(0, 5, 25)), "Game Camera", rootObj);
	renderingData data; 
	data.pFarDist = 25.f; 
	gameCamera = DBG_NEW ComponentCamera(gameCameraObj, vec3(0, 5, 0), data); 
	gameCameraObj->AddComponent(gameCamera);
	
	// Octree
	App->spatial_tree->CreateOctree(math::AABB(float3(-20, 0, -20), float3(20, 40, 20)));

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

// Update
update_status SmileScene::Update(float dt)
{
	rootObj->Update(); 
	DrawObjects();
	HandleGizmo(); 
	DrawGrid();
	DebugLastRay(); 
	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN)
	{
		App->serialization->SaveScene();
	}
	if (App->input->GetKey(SDL_SCANCODE_L) == KEY_DOWN)
	{
		App->serialization->LoadScene("Library/Scenes/scene.json");
	}
	
	return UPDATE_CONTINUE;
}

void SmileScene::DrawObjects()
{
	// collect candidates to be drawn: search for octree nodes inside frustrum 
	std::vector<GameObject*> drawObjects;
	App->spatial_tree->CollectCandidatesA(drawObjects, App->renderer3D->targetCamera->calcFrustrum);

	// then test the own objects OBBs with the frustrum
	App->renderer3D->targetCamera->PruneInsideFrustrum(drawObjects);

	for (auto& obj : drawObjects)
		obj->Draw();

	drawObjects.clear(); 
}

void SmileScene::HandleGizmo()
{
	static ImGuizmo::OPERATION op = ImGuizmo::OPERATION::TRANSLATE;
	if (App->input->GetKey(SDL_SCANCODE_E) == KEY_DOWN)
		op = ImGuizmo::OPERATION::ROTATE;
	if (App->input->GetKey(SDL_SCANCODE_R) == KEY_DOWN)
		op = ImGuizmo::OPERATION::SCALE;

	static ImGuizmo::MODE mode = ImGuizmo::MODE::WORLD; // TODOOO

	if (selectedObj != nullptr)
	{
		ImGuizmo::Enable(true);
		ImGuizmo::SetDrawlist();

		float view[16], projection[16], object[16]; 
		std::memcpy(view, debugCamera->GetViewMatrixTransposed(), sizeof(float) * 16);
		std::memcpy(projection, App->renderer3D->GetProjectionMatrixTransposed(), sizeof(float) * 16);
		std::memcpy(object, selectedObj->GetTransform()->GetGlobalMatrix().Transposed().ptr(), sizeof(float) * 16);

		ImGuiIO& io = ImGui::GetIO();
		ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
		ImGuizmo::Manipulate(view, projection, op, mode, object);
	}
	else
		ImGuizmo::Enable(false);
}
		

void SmileScene::DrawGrid()
{
	glLineWidth(lineWidth);
	glColor3f(1, 1, 1);
	glBegin(GL_LINES);
	for (float i = 0; i <= MAXLINES; i++)
	{
		glVertex3f(i - MAXLINES * linesLength, 0, -MAXLINES * linesLength);
		glVertex3f(i - MAXLINES * linesLength, 0, MAXLINES * linesLength);
		glVertex3f(-MAXLINES * linesLength, 0, i - MAXLINES * linesLength);
		glVertex3f(MAXLINES * linesLength, 0, i - MAXLINES * linesLength);
	}
	glEnd();
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
		ModelMeshData* mesh_info = (mesh) ? mesh->GetMeshData() : nullptr;
		if (mesh_info == nullptr)
			continue;
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
