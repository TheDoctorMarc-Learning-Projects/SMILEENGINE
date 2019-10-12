#include "RayTracer.h"

Mesh* rayTracer::MouseOverMesh(int mouse_x, int mouse_y, bool assignClicked)
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

	for (auto& fbx : App->scene_intro->fbxs)
	{
		for (auto& mesh : fbx->meshes)
		{
			for (int i = 0; i < mesh->num_vertex; i += 9) // 3 vertices * 3 coords (x,y,z) 
			{
				math::float3 v1(mesh->vertex[i], mesh->vertex[i + 1], mesh->vertex[i + 2]);
				math::float3 v2(mesh->vertex[i + 3], mesh->vertex[i + 4], mesh->vertex[i + 5]);
				math::float3 v3(mesh->vertex[i + 6], mesh->vertex[i + 7], mesh->vertex[i + 8]);

				math::Triangle tri = math::Triangle(v1, v2, v3);

				// 6) check if the ray interesects with the face (triangle) 
				if (ray.Intersects(tri))
				{
					foundAt[0] = k;
					foundAt[1] = j;
				}

			}

			++j;
		}
		++k;
	}

	// 7) If clicked inside an object, select it, otherwise unselect the current selected
	if (foundAt[0] != INT_MAX && foundAt[1] != INT_MAX)
	{
		if (assignClicked)
			App->scene_intro->selected_mesh = App->scene_intro->fbxs.at(foundAt[0])->meshes.at(foundAt[1]);

		LOG("Selected Mesh in the scene :)");
		return App->scene_intro->fbxs.at(foundAt[0])->meshes.at(foundAt[1]);
	}
	else
	{
		if (assignClicked)
			App->scene_intro->selected_mesh = nullptr;

		LOG("Unselected Mesh in the scene :o");
		return nullptr;
	}

	return nullptr;
}
