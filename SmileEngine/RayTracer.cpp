#include "RayTracer.h"
#include "SmileApp.h"
#include "SmileGui.h"

ComponentMesh* rayTracer::MouseOverMesh(int mouse_x, int mouse_y, bool assignClicked) // todo: search with root obj recursively
{
	/*// 0) Skip if click was while a gui menu is open
	if (App->gui->IsMouseOverTheGui() == true)
		return nullptr; 

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
	{
		if (assignClicked)
		{
			App->scene_intro->selected_mesh = nullptr;
			LOG("Unselected Mesh in the scene :o");
		}

		return nullptr;
	}

	// 3) Unproject to find the final point coordinates in the world
	GLdouble fPos[3];
	gluUnProject(mouse_X_GL, mouse_Y_GL, mouse_Z_GL, mvMatrix, projMatrix, viewport, &fPos[0], &fPos[1], &fPos[2]);
	math::float3 fPosMath(fPos[0], fPos[1], fPos[2]);

	// 4) trace a ray (line) from the camera with the direction to the point  
	math::float3 cameraPos(App->camera->Position.x, App->camera->Position.y, App->camera->Position.z);
	math::float3 dir = (fPosMath - cameraPos).Normalized();
	math::Ray ray = math::Ray(cameraPos, dir);

	// 5) Loop meshes in the screen and find an intersection
	int foundAt[2] = { INT_MAX, INT_MAX };
	int k = 0, j = 0;

	for (auto& gameObject : App->scene_intro->objects)
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
					foundAt[0] = k;
					foundAt[1] = j;
					goto Resolve; 
				}

			}

			++j;
		}
		++k;
	}

    Resolve:
	// 7) If clicked inside an object, select it, otherwise unselect the current selected
	if (foundAt[0] != INT_MAX && foundAt[1] != INT_MAX)
	{
		ComponentMesh* targetMesh =
			dynamic_cast<ComponentMesh*>(std::get<std::vector<Component*>>(App->scene_intro->objects.at(foundAt[0])->GetComponent(MESH)).at(foundAt[1])); // oh damn beautiful 

		if (assignClicked)
		{
			App->scene_intro->selected_mesh = targetMesh; 
			LOG("Selected Mesh in the scene :)");
		}
		
		return targetMesh; 
	}
	else
	{
		if (assignClicked)
		{
			App->scene_intro->selected_mesh = nullptr;
			LOG("Unselected Mesh in the scene :o");
		}

		return nullptr;
	}*/

	return nullptr;
}
