#include "RayTracer.h"
#include "SmileApp.h"
#include "SmileGui.h"

std::variant<ComponentMesh*, GameObject*> rayTracer::MouseOverMesh(int mouse_x, int mouse_y, bool assignClicked, bool GetMeshNotGameObject)  
{
	// 0) Skip if click was while a gui menu is open
	if (App->gui->IsMouseOverTheGui() == true)
	{
		assignClicked = false; 
		goto EmptyReturn;
	}
	

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

	// To Optimize, if the mouse Z depth is 1.f (the max, in the horizon), skip calculus and return nullptr
	if ((float)mouse_Z_GL == 1.f)
		goto EmptyReturn; 

	// 3) Unproject to find the final point coordinates in the world
	GLdouble fPos[3];
	gluUnProject(mouse_X_GL, mouse_Y_GL, mouse_Z_GL, mvMatrix, projMatrix, viewport, &fPos[0], &fPos[1], &fPos[2]);
	math::float3 fPosMath(fPos[0], fPos[1], fPos[2]);

	// 4) trace a ray (line) from the camera with the direction to the point  
	math::float3 cameraPos(App->camera->Position.x, App->camera->Position.y, App->camera->Position.z);
	math::float3 dir = (fPosMath - cameraPos).Normalized();
	math::Ray ray = math::Ray(cameraPos, dir);

	// 5) Loop meshes in the screen and find an intersection
	ComponentMesh* found = nullptr; 

	for (auto& gameObject : App->scene_intro->rootObj->GetChildrenRecursive())
	{
		std::vector<Component*> meshes = std::get<std::vector<Component*>>(gameObject->GetComponent(MESH));

		for (auto& mesh : meshes)
		{
			ModelMeshData* mesh_info = dynamic_cast<ComponentMesh*>(mesh)->GetMeshData(); 

			for (int i = 0; i < mesh_info->num_vertex; i += 9) // 3 vertices * 3 coords (x,y,z) 
			{
				math::float3 v1(mesh_info->vertex[i], mesh_info->vertex[i + 1], mesh_info->vertex[i + 2]);
				math::float3 v2(mesh_info->vertex[i + 3], mesh_info->vertex[i + 4], mesh_info->vertex[i + 5]);
				math::float3 v3(mesh_info->vertex[i + 6], mesh_info->vertex[i + 7], mesh_info->vertex[i + 8]);

				math::Triangle tri = math::Triangle(v1, v2, v3);

				// 6) check if the ray interesects with the face (triangle) 
				if (ray.Intersects(tri))
				{
					found = dynamic_cast<ComponentMesh*>(mesh); 
					goto Resolve; 
				}

			}
		}
	}

Resolve:

	// A) the user just wants to know if the mouse is over a mesh or gameobject
	if (assignClicked == false)
	{
		// if the user asked for the mesh
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

	// B) the user wants to select a mesh or object
	if (found != nullptr)
	{
		GameObject* selectedObj = App->scene_intro->selectedObj;
		ComponentMesh* selectedMesh = App->scene_intro->selected_mesh;

		// A) if nothing is selected, or another mesh / object is selected, select the mesh parent (object).
		// Do not select the parent if another mesh of the same parent is selected, in that case select the mesh 
		if (
			(!selectedObj && !selectedMesh)
			|| (selectedObj && selectedObj != found->GetParent())
			|| (selectedMesh && selectedMesh->GetParent() != found->GetParent())
			)
		{

			App->scene_intro->selectedObj = found->GetParent();
			App->scene_intro->selected_mesh = nullptr;
			LOG("Selected GameObject in the scene :)");

	
			// Return nullptr is the user asked for a mesh, or the object is the user asked for an object
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


			// Return nullptr is the user asked for a object, or the mesh if the user asked for a mesh
			if (GetMeshNotGameObject)
				return (ComponentMesh*)App->scene_intro->selected_mesh;
			else
				return (GameObject*)nullptr;

		}
			
	
		
	}
	else
		goto EmptyReturn;  // this could be skipped but I leave it to make it more understandable  


    EmptyReturn:
	// unselect all, no mather if it was a mesh or an object
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
