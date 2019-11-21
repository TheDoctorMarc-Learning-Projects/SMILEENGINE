#include "ResourceMesh.h"
#include "Glew/include/GL/glew.h" 
#include "DevIL/include/IL/ilu.h"

void ResourceMesh::LoadOnMemory()
{
	// Normals Buffer
	glGenBuffers(1, (GLuint*) &buffers[mesh_b_normals]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[mesh_b_normals]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(float) * buffersSize[mesh_b_normals] * 3, normals, GL_STATIC_DRAW);

	// Uvs vBuffer
	glGenBuffers(1, (GLuint*) &buffers[mesh_b_UVs]);
	glBindBuffer(GL_ARRAY_BUFFER, buffers[mesh_b_UVs]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * buffersSize[mesh_b_UVs] * 2, UVs, GL_STATIC_DRAW);

	// Color Buffer
	glGenBuffers(1, (GLuint*) &buffers[mesh_b_colors]);
	glBindBuffer(GL_ARRAY_BUFFER, buffers[mesh_b_colors]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * buffersSize[mesh_b_colors] * 3, colors, GL_STATIC_DRAW);

	// Vertex Buffer
	glGenBuffers(1, (GLuint*) &buffers[mesh_b_vertex]);
	glBindBuffer(GL_ARRAY_BUFFER, buffers[mesh_b_vertex]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * buffersSize[mesh_b_vertex] * 3, vertex, GL_STATIC_DRAW);

	// Index Buffer
	glGenBuffers(1, (GLuint*) &buffers[mesh_b_index]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[mesh_b_index]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * buffersSize[mesh_b_index], index, GL_STATIC_DRAW);
}

void ResourceMesh::FreeMemory()
{
	if (buffers != nullptr)
	{
		if (&buffers[mesh_b_vertex] != nullptr)
		{
			glDeleteBuffers(1, (GLuint*)&vertex);
			RELEASE_ARRAY(vertex);
		}

		if (&buffers[mesh_b_index] != nullptr)
		{
			glDeleteBuffers(1, (GLuint*)&index);
			RELEASE_ARRAY(index);
		}

		if (&buffers[mesh_b_normals] != nullptr)
		{
			glDeleteBuffers(1, (GLuint*)&normals);
			RELEASE_ARRAY(normals);
		}

		if (&buffers[mesh_b_colors] != nullptr)
		{
			glDeleteBuffers(1, (GLuint*)&colors);
			RELEASE_ARRAY(colors);
		}

		if (&buffers[mesh_b_UVs] != nullptr)
		{
			glDeleteBuffers(1, (GLuint*)&UVs);
			RELEASE_ARRAY(UVs);
		}

		//RELEASE();
	}
}
