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
	float2 NormalizeMousePos(int x, int y); 
	math::LineSegment TraceRay(float2 normalizedMousePos);
	ComponentMesh* GetClosestIntersection(math::LineSegment ray);
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
	const float2 normalizedMousePos = internal::NormalizeMousePos(mouse_x, mouse_y);
	
	// 2) Calculate a ray from the camera to the mouse pos in the world
	math::LineSegment ray = internal::TraceRay(normalizedMousePos);

	// 3) Loop meshes in the screen and find an intersection with the ray
	ComponentMesh* found = internal::GetClosestIntersection(ray);

	// 4) Finally, return a mesh or a GameObject depending on the user's needs.
	// Only assign a selected mesh or Gameobject if requested. 
	if (assignClicked == false)
		return internal::ReturnHover(GetMeshNotGameObject, found);
	else
		return internal::ReturnClick(GetMeshNotGameObject, found);
	
}


// ----------------------------------------------------------------- 
float2 internal::NormalizeMousePos(int mouse_x, int mouse_y)
{
	// 1) Translate from window coordinates to inverted Y in OpenGL 
	float windowW = (float)std::get<int>(App->window->GetWindowParameter("Width")); 
	float windowH = (float)std::get<int>(App->window->GetWindowParameter("Height"));
	float2 mousePos(mouse_x, windowH - mouse_y);

	// 2) Translate to normalized coords
	float2 normMousPos = mousePos; 
	normMousPos.x = (2.F * mousePos.x) / windowW - 1.F;
	normMousPos.y = 1.F - (2.F * mousePos.y) / windowH;

	return normMousPos;
}


// ----------------------------------------------------------------- 
math::LineSegment internal::TraceRay(float2 normMousePos)
{
	return App->renderer3D->targetCamera->calcFrustrum.UnProjectLineSegment(normMousePos.x, normMousePos.y); 
}


// -----------------------------------------------------------------  
ComponentMesh* internal::GetClosestIntersection(math::LineSegment ray)
{
	// TODO: collect objects inside octree nodes and ultimately inside frustrum 
	auto objects = App->scene_intro->rootObj->GetChildrenRecursive(); 

	for (auto& gameObject : objects)
	{
		// Get the mesh and find an intersection
		ComponentMesh* mesh = dynamic_cast<ComponentMesh*>(gameObject->GetComponent(MESH)); 
		ModelMeshData* mesh_info = (mesh) ? mesh->GetMeshData() : nullptr;
		if (mesh_info == nullptr)
			continue; 

		for (int i = 0; i < mesh_info->num_vertex; i += 9) // 3 vertices * 3 coords (x,y,z) 
		{
			math::float3 v1(mesh_info->vertex[i], mesh_info->vertex[i + 1], mesh_info->vertex[i + 2]);
			math::float3 v2(mesh_info->vertex[i + 3], mesh_info->vertex[i + 4], mesh_info->vertex[i + 5]);
			math::float3 v3(mesh_info->vertex[i + 6], mesh_info->vertex[i + 7], mesh_info->vertex[i + 8]);

			// form a triangle and translate to global coords to test with ray!
			math::Triangle tri = math::Triangle(v1, v2, v3);

			// Finally check if the ray interesects with the face (triangle) 
			if (ray.Intersects(tri, nullptr, nullptr))
			{
				// TODO: push check the closest, keep searching 
				// return dynamic_cast<ComponentMesh*>(mesh);
				int allahu_akbar = 0; 
			}

		}
	}

	return nullptr; 
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


