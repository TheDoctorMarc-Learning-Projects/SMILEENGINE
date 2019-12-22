#include "ComponentMesh.h"

#include "Glew/include/GL/glew.h" 
#include "DevIL/include/IL/ilu.h"
#include "SmileApp.h"
#include "SmileFBX.h"
#include "Component.h"
#include "ComponentTransform.h"
#include "ComponentMaterial.h"
#include "SmileResourceManager.h"
#include "ResourceMesh.h"
#include "SmileApp.h"



ComponentMesh::ComponentMesh(SmileUUID uid, std::string name)
{
	SetName(name);
	type = MESH;
	meshType = Mesh_Type::MODEL;
	myresourceID = uid;

	// update reference counting in resource
	App->resources->UpdateResourceReferenceCount(uid, 1); 

}

void ComponentMesh::Enable() // Called in "AddComponent()" from GameObject 
{

}

ComponentMesh::~ComponentMesh()
{
	
}

void ComponentMesh::Draw()
{
	auto resource = dynamic_cast<ResourceMesh*>(App->resources->Get(myresourceID)); 
	auto mesh_data = resource->GetMeshData(); 
	DrawBegin();
	(mesh_data.index() == 0) ? DefaultDraw(std::get<ModelMeshData*>(mesh_data)) : OwnDraw(std::get<ownMeshData*>(mesh_data));
	DrawEnd(); 
}

void ComponentMesh::DrawBegin()
{
	// Transformation  
	glPushMatrix();
	glMultMatrixf(dynamic_cast<ComponentTransform*>(parent->GetComponent(TRANSFORM))->GetGlobalMatrix().Transposed().ptr());

	// Cient states
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnable(GL_ALPHA_TEST);
}

void ComponentMesh::DrawEnd()
{
	// Disable Cient states && clear data
	glColor3f(1.0f, 1.0f, 1.0f);
	glAlphaFunc(GL_EQUAL, (GLclampf)1.f);
	glDisable(GL_ALPHA_TEST);
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);

	// Debug on top
	/*if (App->scene_intro->generalDbug)
		DebugDraw();*/

	// Transformation
	glPopMatrix();
}

void ComponentMesh::DefaultDraw(ModelMeshData* model_mesh)
{
	// vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, model_mesh->id_vertex);
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	// index buffer 
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model_mesh->id_index);

	// Material
	ComponentMaterial* mat = dynamic_cast<ComponentMaterial*>(parent->GetComponent(MATERIAL));
	if (mat != nullptr)
	{
		// UVs buffer
		if (model_mesh->UVs != nullptr)
		{
			// texture buffer
			glBindTexture(GL_TEXTURE_2D, mat->GetTextureData()->id_texture);

			// Alpha Testing
			glAlphaFunc(GL_GREATER, (GLclampf)GetParent()->GetMaterial()->GetMaterialData()->transparency);

			// Uvs
			glBindBuffer(GL_ARRAY_BUFFER, model_mesh->id_UVs);
			glTexCoordPointer(2, GL_FLOAT, 0, NULL);

		}

	}

	// normal buffer
	if (model_mesh->normals != nullptr)
	{
		glBindBuffer(GL_NORMAL_ARRAY, model_mesh->id_normals);
		glNormalPointer(GL_FLOAT, 0, NULL);
	}

	// Draw // unsigned short for primitives???
	glDrawElements(GL_TRIANGLES, model_mesh->num_index * 3, GL_UNSIGNED_INT, NULL);
}

void ComponentMesh::OwnDraw(ownMeshData* data)
{
	// Material
	ComponentMaterial* mat = dynamic_cast<ComponentMaterial*>(parent->GetComponent(MATERIAL));

	if (mat)
	{
		glBindTexture(GL_TEXTURE_2D, mat->GetTextureData()->id_texture);
		// Alpha Testing
		glAlphaFunc(GL_GREATER, (GLclampf)GetParent()->GetMaterial()->GetMaterialData()->transparency);
	}
		

	switch (data->type)
	{
	case ownMeshType::plane:
	{
		glBegin(GL_QUADS); 
		for (int i = 0; i < data->points.size(); i += 2)
		{
			if(mat && !data->uvCoords.empty())
				glTexCoord2f(data->uvCoords.at(i), data->uvCoords.at(i + 1)); 
		
			glVertex2f(data->points.at(i), data->points.at(i + 1));
		}
		glEnd(); 
		break; 
	}
	default:
		break;
	}

	if (mat)
		glBindTexture(GL_TEXTURE_2D,0);


}

void ComponentMesh::DebugDraw() // should consider the new mesh type
{
/*	auto model_mesh = dynamic_cast<ResourceMesh*>(App->resources->Get(myresourceID))->model_mesh;
		if (model_mesh->normals != nullptr)
		{
			// draw vertex normals
			if (debugData.vertexNormals)
			{
				glColor3f(0.f, 1.0f, 0.f);

				static float normalFactor = 20.f;

				for (int i = 0; i < model_mesh->num_normals * 3; i += 3)
				{
					glBegin(GL_LINES);

					vec3 normalVec = normalize({ model_mesh->normals[i], model_mesh->normals[i + 1], model_mesh->normals[i + 2] });
					glVertex3f(model_mesh->vertex[i], model_mesh->vertex[i + 1], model_mesh->vertex[i + 2]);
					glVertex3f(model_mesh->vertex[i] + normalVec.x, model_mesh->vertex[i + 1] + normalVec.y, model_mesh->vertex[i + 2] + normalVec.z);

					glEnd();
				}

				glColor3f(1.0f, 1.0f, 1.0f);
			}

			// draw face normals
			if (debugData.faceNormals)
			{
				glColor3f(0.f, 1.0f, 0.f);

				float size = 5.f;
				for (int i = 0; i < model_mesh->num_normals; i += 3) {

					glBegin(GL_LINES);
					glColor3f(0, 1, 0);


					float vec1_x = model_mesh->vertex[model_mesh->index[i] * 3];
					float vec1_y = model_mesh->vertex[(model_mesh->index[i] * 3) + 1];
					float vec1_z = model_mesh->vertex[(model_mesh->index[i] * 3) + 2];


					float vec2_x = model_mesh->vertex[model_mesh->index[i + 1] * 3];
					float vec2_y = model_mesh->vertex[(model_mesh->index[i + 1] * 3) + 1];
					float vec2_z = model_mesh->vertex[(model_mesh->index[i + 1] * 3) + 2];

					float vec3_x = model_mesh->vertex[model_mesh->index[i + 2] * 3];
					float vec3_y = model_mesh->vertex[(model_mesh->index[i + 2] * 3) + 1];
					float vec3_z = model_mesh->vertex[(model_mesh->index[i + 2] * 3) + 2];

					float mid_x = (vec1_x + vec2_x + vec3_x) / 3;
					float mid_y = (vec1_y + vec2_y + vec3_y) / 3;
					float mid_z = (vec1_z + vec2_z + vec3_z) / 3;

					vec3 face_center = { mid_x, mid_y, mid_z };

					vec3 vec_v1_center = face_center - vec3(vec1_x, vec1_y, vec1_z);
					vec3 vec_v2_center = face_center - vec3(vec2_x, vec2_y, vec2_z);

					vec3 normal_vec = cross(vec_v1_center, vec_v2_center);
					vec3 normalized_normal_vec = normalize(normal_vec);

					glVertex3f(mid_x, mid_y, mid_z);

					glVertex3f(mid_x + normalized_normal_vec.x * size, mid_y + normalized_normal_vec.y * size, mid_z + normalized_normal_vec.z * size);

					glEnd();
				}

				glColor3f(1.0f, 1.0f, 1.0f);
			}
		}

		// Debug AABB
		/*if (this->debugData.AABB) // use the math obb corners 
		{
			glColor3f(1.0f, 0.0f, 0.0f);
			glPointSize(10); 
			glBegin(GL_POINTS);
			auto AABB = GetParent()->GetBoundingData().AABB; 
			for (int i = 0; i < AABB.vertices.size(); ++i)
			{
				if(AABB.insideoutside.at(i) == true)
					glColor3f(0.0f, 1.0f, 0.0f);
				else
					glColor3f(1.0f, 0.0f, 0.0f);

				glVertex3f(AABB.vertices.at(i).x, AABB.vertices.at(i).y, AABB.vertices.at(i).z);
			}
			
			glEnd();
			glPointSize(1);
			glColor3f(1.0f, 1.0f, 1.0f);
		}*/
		
	
}



// -----------------------------------------------------------------
void ComponentMesh::CleanUp()
{

	// update reference counting in resource
	App->resources->UpdateResourceReferenceCount(myresourceID, -1);

}

 
void ComponentMesh::OnTransform(bool data[3])
{
	
}

ResourceMesh* ComponentMesh::GetResourceMesh()
{
	return dynamic_cast<ResourceMesh*>(App->resources->Get(myresourceID));
}
