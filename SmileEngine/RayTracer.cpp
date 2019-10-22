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
	bool IsMouseInsideEntityRadius(std::variant<ComponentMesh*, GameObject*> entity, float3 mousePos3D);

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
	// 0) Skip if click was while a gui menu is open
	if (App->gui->IsMouseOverTheGui() == true)
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
	// trace a ray (line) from the camera with the direction to the point  
	math::float3 cameraPos(App->camera->Position.x, App->camera->Position.y, App->camera->Position.z);
	math::float3 dir = (mouse3Dpos - cameraPos).Normalized();
	return math::Ray(cameraPos, dir);
}


// ----------------------------------------------------------------- 
ComponentMesh* internal::GetIntersectedMesh(float3 mouse3Dpos, math::Ray ray)
{
	for (auto& gameObject : App->scene_intro->rootObj->GetChildrenRecursive())
	{
		// 1.A) SKIP the object directly if the mouse point distance to the object's center is greater than the object radius.
		if (internal::IsMouseInsideEntityRadius(gameObject, mouse3Dpos) == false)
			continue;

		// 1.B) DO NOT SKIP if the mouse point falls inside the bounding sphere radius.
		// Loop the meshes

		std::vector<Component*> meshes = std::get<std::vector<Component*>>(gameObject->GetComponent(MESH));
		for (auto& mesh : meshes)
		{
			ComponentMesh* realMesh = dynamic_cast<ComponentMesh*>(mesh); 
			ModelMeshData* mesh_info = realMesh->GetMeshData(); 

			// 2.A) SKIP the mesh directly if the mouse point distance to the mesh center is greater than the mesh's radius.
			if (internal::IsMouseInsideEntityRadius(realMesh, mouse3Dpos) == false)
				continue;

			// 2.B) DO NOT SKIP if the mouse point falls inside the bounding sphere radius.
			// Loop the mesh triangles and find an intersection with the ray
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
	}

	return nullptr; 
}


// ----------------------------------------------------------------- 
bool internal::IsMouseInsideEntityRadius(std::variant<ComponentMesh*, GameObject*> entity, float3 mousePos3D)
{
	double distMouseToObj = (double)INFINITE; 
	double radius = 0; 

	if (entity.index() == 0) // mesh 
	{
		auto mesh = std::get<ComponentMesh*>(entity);
		auto transf = dynamic_cast<ComponentTransform*>(std::get<Component*>(mesh->GetComponent(TRANSFORM)));
		distMouseToObj = abs((mousePos3D - transf->GetPosition()).Length());
		radius = mesh->GetMeshData()->GetMeshSphereRadius();
	}
	else if (entity.index() == 1) // object
	{
		auto obj = std::get<GameObject*>(entity);
		auto transf = dynamic_cast<ComponentTransform*>(std::get<Component*>(obj->GetComponent(TRANSFORM)));
		distMouseToObj = abs((mousePos3D - transf->GetPosition()).Length());
		radius = obj->GetBoundingSphereRadius();
	}

	// Return 
	if (distMouseToObj <= radius)
		return true;
	else
		return false;
	
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

