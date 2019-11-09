#include "RayTracer.h"
#include "SmileApp.h"
#include "SmileGui.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "Component.h"

// ----------------------------------------------------------------- internal (private) space for the computation:
namespace internal
{
	// Calculus
	float3 GetMouse3DPos(int mouse_x, int mouse_y);
	math::Ray GetRayBetweenCameraAndMouse3DPos(float3 mousePos3D);
	ComponentMesh* GetIntersectedMesh(float3 mousePos3D, math::Ray ray);
	bool IsMouseInsideEntityRadius(GameObject* obj, float3 mousePos3D);

	// Skipper conditions: returns true to skip the object if it has no mesh or if it is too far from mouse
	bool SkipperConditions(GameObject* obj, float3 mouse3Dpos);

	// General skipper conditions: returns true if mouse is in the dark horizon or if alt+click is active (orbiting object)
	bool GeneralSkipperConditions(); 

	// Return and assignment
	std::variant<ComponentMesh*, GameObject*> ReturnHover(bool GetMeshNotGameObject, ComponentMesh* found);
	std::variant<ComponentMesh*, GameObject*> ReturnClick(bool GetMeshNotGameObject, ComponentMesh* found);
	std::variant<ComponentMesh*, GameObject*> EmptyReturn(bool GetMeshNotGameObject, bool assignClicked);
}


// ----------------------------------------------------------------- USE THIS FUNCTION :)
// ----------------------------------------------------------------- The first bool = hover or click
// ----------------------------------------------------------------- The second bool = return a mesh or an object
std::variant<ComponentMesh*, GameObject*> rayTracer::MouseOverMesh(int mouse_x, int mouse_y, bool assignClicked, bool GetMeshNotGameObject)  
{
	// 0) Skip if general conditions are present (see the function)
	if (internal::GeneralSkipperConditions() == true)
	{
		assignClicked = false; 
		return internal::EmptyReturn(GetMeshNotGameObject, assignClicked);
	}

	// 1) Calculate the mouse pos in the world
	const float3 mousePos = internal::GetMouse3DPos(mouse_x, mouse_y);
	
	if (mousePos.IsFinite() == false)
		return internal::EmptyReturn(GetMeshNotGameObject, assignClicked);
	
	// 2) Calculate a ray from the camera to the mouse pos in the world
	math::Ray ray = internal::GetRayBetweenCameraAndMouse3DPos(mousePos);

	// 3) Loop meshes in the screen and find an intersection with the ray
	ComponentMesh* found = internal::GetIntersectedMesh(mousePos, ray);

	// 4) Finally, return a mesh or a GameObject depending on the user's needs.
	// Only assign a selected mesh or Gameobject if requested. 
	if (assignClicked == false)
		return internal::ReturnHover(GetMeshNotGameObject, found);
	else
		return internal::ReturnClick(GetMeshNotGameObject, found);
	
}


// ----------------------------------------------------------------- 
float3 internal::GetMouse3DPos(int mouse_x, int mouse_y)
{
	// 1) Translate from window coordinates to inverted Y in OpenGL 
	float mouse_X_GL, mouse_Y_GL;
	GLfloat mouse_Z_GL;
	mouse_X_GL = mouse_x;
	mouse_Y_GL = std::get<int>(App->window->GetWindowParameter("Height")) - mouse_y;

	// 2) Get matrixes and also a Z component to the mouse click
	GLint viewport[4];
	GLdouble projMatrix[16];
	GLdouble mvMatrix[16];

	glGetIntegerv(GL_VIEWPORT, viewport);
	glGetDoublev(GL_MODELVIEW_MATRIX, mvMatrix);
	glGetDoublev(GL_PROJECTION_MATRIX, projMatrix);

	glReadPixels(mouse_X_GL, mouse_Y_GL, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &mouse_Z_GL);

	// To Optimize, if the mouse Z depth is 1.f (the max, in the horizon), skip calculus and return infinite 
	if ((float)mouse_Z_GL == 1.f)
		return float3::inf; 

	// 3) Unproject to find the final point coordinates in the world
	GLdouble fPos[3];
	gluUnProject(mouse_X_GL, mouse_Y_GL, mouse_Z_GL, mvMatrix, projMatrix, viewport, &fPos[0], &fPos[1], &fPos[2]);
	math::float3 fPosMath(fPos[0], fPos[1], fPos[2]);

	return fPosMath; 
}


// ----------------------------------------------------------------- 
math::Ray internal::GetRayBetweenCameraAndMouse3DPos(float3 mouse3Dpos)
{
	math::float3 camPos = App->scene_intro->debugCamera->GetParent()->GetTransform()->GetPosition(); 
	// trace a ray (line) from the camera with the direction to the point  
	math::float3 dir = (mouse3Dpos - camPos).Normalized();
	return math::Ray(camPos, dir);
}


// ----------------------------------------------------------------- 
ComponentMesh* internal::GetIntersectedMesh(float3 mouse3Dpos, math::Ray ray)
{
	for (auto& gameObject : App->scene_intro->rootObj->GetChildrenRecursive())
	{
		// Check conditions to optimize and discard the object
		if (internal::SkipperConditions(gameObject, mouse3Dpos))
			continue; 

		// Do not skip: we assume it has a mesh (already checked)
		ComponentMesh* mesh = dynamic_cast<ComponentMesh*>(gameObject->GetComponent(MESH)); 
		ModelMeshData* mesh_info = mesh->GetMeshData();
		for (int i = 0; i < mesh_info->num_vertex; i += 9) // 3 vertices * 3 coords (x,y,z) 
		{
			math::float3 v1(mesh_info->vertex[i], mesh_info->vertex[i + 1], mesh_info->vertex[i + 2]);
			math::float3 v2(mesh_info->vertex[i + 3], mesh_info->vertex[i + 4], mesh_info->vertex[i + 5]);
			math::float3 v3(mesh_info->vertex[i + 6], mesh_info->vertex[i + 7], mesh_info->vertex[i + 8]);

			// form a triangle
			math::Triangle tri = math::Triangle(v1, v2, v3);

			// Finally check if the ray interesects with the face (triangle) 
			if (ray.Intersects(tri))
			{
				return dynamic_cast<ComponentMesh*>(mesh);
			}

		}
	}

	return nullptr; 
}


// ----------------------------------------------------------------- 
bool internal::IsMouseInsideEntityRadius(GameObject* obj, float3 mousePos3D)
{
	auto transf = dynamic_cast<ComponentTransform*>(obj->GetComponent(TRANSFORM));
	double distMouseToObj = abs((mousePos3D - transf->GetPosition()).Length());
	double radius = obj->GetBoundingSphereRadius();

	if (distMouseToObj <= radius)
		return true;
	else
		return false;
}

bool internal::GeneralSkipperConditions()
{
	bool ret = false; 

	// Skip if orbiting object
	if (App->input->GetKey(SDL_SCANCODE_LALT) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT)
		ret = true;

	// Skip if mouse in the gui
	if (App->gui->IsMouseOverTheGui())
		ret = true; 

	return ret; 
}
// ----------------------------------------------------------------- 
bool internal::SkipperConditions(GameObject* obj, float3 mouse3Dpos)
{
	bool ret = false; 

	// 1.A) SKIP the object directly if the mouse point distance to the object's center is greater than the object radius.
	if (internal::IsMouseInsideEntityRadius(obj, mouse3Dpos) == false)
		ret = true;

	// 1.B) SKIP if the object has no mesh
	auto mesh = dynamic_cast<ComponentMesh*>(obj->GetComponent(MESH));
	if (mesh == nullptr)
		ret = true;

	return ret; 
}

// ----------------------------------------------------------------- 
std::variant<ComponentMesh*, GameObject*> internal::ReturnHover(bool GetMeshNotGameObject, ComponentMesh* found)
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


// ----------------------------------------------------------------- 
std::variant<ComponentMesh*, GameObject*> internal::ReturnClick(bool GetMeshNotGameObject, ComponentMesh* found)
{
	if (found != nullptr)
	{
		GameObject* selectedObj = App->scene_intro->selectedObj;
		ComponentMesh* selectedMesh = App->scene_intro->selected_mesh;

		// A) if nothing is selected, or another mesh / object is selected, select the mesh parent (object).
		// Do not select the parent if another mesh of the same parent is selected, in that case select the mesh 
		// Eg: One Click = select the object. Another clicks = select mesh
		if (
			(!selectedObj && !selectedMesh)
			|| (selectedObj && selectedObj != found->GetParent())
			|| (selectedMesh && selectedMesh->GetParent() != found->GetParent())
			)
		{

			App->scene_intro->selectedObj = found->GetParent();
			App->scene_intro->selected_mesh = nullptr;
			LOG("Selected GameObject in the scene :)");

			// Return nullptr if the user asked for a mesh, or the object is the user asked for an object
			if (GetMeshNotGameObject)
				return (ComponentMesh*)nullptr;
			else
				return (GameObject*)App->scene_intro->selectedObj;

		}

		else // B) if the object parent is selected, or another mesh of the parent, then select the mesh 
		{
			App->scene_intro->selected_mesh = found;
			App->scene_intro->selectedObj = nullptr;
			LOG("Selected Mesh in the scene :)");

			// Return nullptr if the user asked for a object, or the mesh if the user asked for a mesh
			if (GetMeshNotGameObject)
				return (ComponentMesh*)App->scene_intro->selected_mesh;
			else
				return (GameObject*)nullptr;

		}

	}
	else
		return internal::EmptyReturn(GetMeshNotGameObject, true);   
}


// ----------------------------------------------------------------- 
std::variant<ComponentMesh*, GameObject*> internal::EmptyReturn(bool GetMeshNotGameObject, bool assignClicked)
{
	// Unselect the current mesh or object if specified (eg if mouse clicks in the "black empty space in the horizon") 
	if (assignClicked)
	{
		App->scene_intro->selected_mesh = nullptr;
		App->scene_intro->selectedObj = nullptr;
		LOG("Unselected Mesh or Object in the scene :o");
	}

	if (GetMeshNotGameObject)
		return (ComponentMesh*)nullptr;
	else
		return (GameObject*)nullptr;
}


