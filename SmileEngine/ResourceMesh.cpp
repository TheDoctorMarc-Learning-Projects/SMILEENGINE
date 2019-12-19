#include "ResourceMesh.h"
#include "Glew/include/GL/glew.h" 
#include "DevIL/include/IL/ilu.h"


void ResourceMesh::LoadOnMemory(const char* path)
{
	// Normals Buffer
	glGenBuffers(1, (GLuint*) & (model_mesh->id_normals));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model_mesh->id_normals);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(float) * model_mesh->num_normals * 3, model_mesh->normals, GL_STATIC_DRAW);

	// Uvs vBuffer
	glGenBuffers(1, (GLuint*) & (model_mesh->id_UVs));
	glBindBuffer(GL_ARRAY_BUFFER, model_mesh->id_UVs);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * model_mesh->num_UVs * 2, model_mesh->UVs, GL_STATIC_DRAW);

	// Color Buffer
	glGenBuffers(1, (GLuint*) & (model_mesh->id_color));
	glBindBuffer(GL_ARRAY_BUFFER, model_mesh->id_color);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * model_mesh->num_color * 3, model_mesh->color, GL_STATIC_DRAW);

	// Vertex Buffer
	glGenBuffers(1, (GLuint*) & (model_mesh->id_vertex));
	glBindBuffer(GL_ARRAY_BUFFER, model_mesh->id_vertex);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * model_mesh->num_vertex * 3, model_mesh->vertex, GL_STATIC_DRAW);

	// Index Buffer
	glGenBuffers(1, (GLuint*) & (model_mesh->id_index));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model_mesh->id_index);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * model_mesh->num_index, model_mesh->index, GL_STATIC_DRAW);
}

void ResourceMesh::FreeMemory()
{
	// Free model mesh
	if (model_mesh != nullptr)
	{
		if (model_mesh->vertex != nullptr)
		{
			glDeleteBuffers(1, (GLuint*)&model_mesh->vertex);
			RELEASE_ARRAY(model_mesh->vertex);
		}

		if (model_mesh->index != nullptr)
		{
			glDeleteBuffers(1, (GLuint*)&model_mesh->index);
			RELEASE_ARRAY(model_mesh->index);
		}

		if (model_mesh->normals != nullptr)
		{
			glDeleteBuffers(1, (GLuint*)&model_mesh->normals);
			RELEASE_ARRAY(model_mesh->normals);
		}

		if (model_mesh->color != nullptr)
		{
			glDeleteBuffers(1, (GLuint*)&model_mesh->color);
			RELEASE_ARRAY(model_mesh->color);
		}

		if (model_mesh->UVs != nullptr)
		{
			glDeleteBuffers(1, (GLuint*)&model_mesh->UVs);
			RELEASE_ARRAY(model_mesh->UVs);
		}

		RELEASE(model_mesh);
	}


	if (own_mesh != nullptr)
		RELEASE(own_mesh); 
}

AABB ResourceMesh::GetEnclosingAABB()
{
	math::AABB ret;
	ret.SetNegativeInfinity();
	ret.Enclose((math::float3*)model_mesh->vertex, model_mesh->num_vertex);
	return ret; 
}

void ResourceMesh::GenerateModelMeshFromParShapes(par_shapes_mesh* mesh)
{
	if (model_mesh == nullptr)
	{
		par_shapes_unweld(mesh, true);
		par_shapes_compute_normals(mesh);
		par_shapes_translate(mesh, 0.f, 0.f, 0.f); // TODO: do this with the gameobject transform

		model_mesh = DBG_NEW ModelMeshData();

		model_mesh->num_vertex = mesh->npoints;
		model_mesh->vertex = new float[model_mesh->num_vertex * 3];
		memcpy(model_mesh->vertex, mesh->points, sizeof(float) * model_mesh->num_vertex * 3);

		model_mesh->num_index = mesh->ntriangles * 3;
		model_mesh->index = new uint[model_mesh->num_index];
		memcpy(model_mesh->index, mesh->triangles, sizeof(uint) * model_mesh->num_index);

		if (mesh->normals != nullptr)
		{
			model_mesh->num_normals = model_mesh->num_vertex;
			model_mesh->normals = new float[model_mesh->num_vertex * 3];
			memcpy(model_mesh->normals, mesh->normals, sizeof(float) * model_mesh->num_vertex * 3);
		}

		if (mesh->tcoords != nullptr)
		{
			model_mesh->num_UVs = model_mesh->num_vertex;
			model_mesh->UVs = new float[model_mesh->num_vertex * 2];
			memcpy(model_mesh->UVs, mesh->tcoords, sizeof(float) * model_mesh->num_UVs);
		}

		// Generate Mesh Buffers
		LoadOnMemory();

	}

	par_shapes_free_mesh(mesh);
}

void ResourceMesh::GenerateModelMeshFromOwnType(ownMeshType type)
{
	if (own_mesh != nullptr)
		return;
	own_mesh = DBG_NEW ownMeshData;
	own_mesh->type = type; 

	switch (type)
	{
	case plane:
		own_mesh->size = 1.f; 
		own_mesh->points = { 0, own_mesh->size, 0, 0, own_mesh->size, 0, own_mesh->size, own_mesh->size};
		own_mesh->uvCoords = { 0,1,0,0,1,0,1,1 };
		break;
	case no_type:
		break;
	default:
		break;
	}
}
