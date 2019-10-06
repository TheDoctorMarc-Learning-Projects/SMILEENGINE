#include "SmileApp.h"
#include "SmileFBX.h"
#include "Glew/include/GL/glew.h" 
#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"


#pragma comment (lib, "Assimp/libx86/assimp.lib")



SmileFBX::SmileFBX(SmileApp* app, bool start_enabled) : SmileModule(app, start_enabled) 
{

}

SmileFBX::~SmileFBX() 
{}

bool SmileFBX::Start()
{
	bool ret = true;
	struct aiLogStream stream;
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	aiAttachLogStream(&stream);

	return ret;
}

bool SmileFBX::CleanUp()
{
	aiDetachAllLogStreams();
	return true;
}

void SmileFBX::ReadMeshData(const char* path) {

	const aiScene* scene = aiImportFile(path, aiProcessPreset_TargetRealtime_MaxQuality);

	if (scene != nullptr && scene->HasMeshes()) 
	{

		for (int i = 0; i < scene->mNumMeshes; ++i) 
		{
			aiMesh* new_mesh = scene->mMeshes[i];
			mesh_info.num_vertex = new_mesh->mNumVertices;
			mesh_info.vertex = new float[mesh_info.num_vertex * 3];
			memcpy(mesh_info.vertex, new_mesh->mVertices, sizeof(float) * mesh_info.num_vertex * 3);
			LOG("New Mesh with %d vertices", mesh_info.num_vertex);
			if (new_mesh->HasFaces())
			{
				mesh_info.num_index = new_mesh->mNumFaces * 3;
				mesh_info.index = new uint[mesh_info.num_index];
				for(uint i = 0; i< new_mesh->mNumFaces; ++i)
				{
					if (new_mesh->mFaces[i].mNumIndices != 3)
					{
						LOG("WARNING, geometry face with != 3 indices!");
					}
					else
					{
						memcpy(&mesh_info.index[i * 3], new_mesh->mFaces[i].mIndices, 3 * sizeof(float));
					}
					
				}
			}
			glGenBuffers(1, (GLuint*) & (mesh_info.id_vertex));
			glBindBuffer(GL_ARRAY_BUFFER, mesh_info.id_vertex);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh_info.num_vertex * 3, mesh_info.vertex, GL_STATIC_DRAW);

			// Index buffer
			glGenBuffers(1, (GLuint*) & (mesh_info.id_index));
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh_info.id_index);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(float) * mesh_info.num_index * 3, mesh_info.index, GL_STATIC_DRAW);

			App->renderer3D->meshes.push_back(&mesh_info);
		}
		// Vertex Buffer
		

		aiReleaseImport(scene);
	}
	else
	{
		LOG("Error loading FBX %s", path);
	}
}

void SmileFBX::DrawFBX(Mesh* mesh)
{

	glEnableClientState(GL_VERTEX_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, mesh_info.id_vertex);
	glVertexPointer(3, GL_FLOAT, 0, NULL);


	glBindBuffer(GL_ARRAY_BUFFER, mesh_info.id_index);

	glDrawElements(GL_TRIANGLES, mesh_info.num_index, GL_UNSIGNED_SHORT, NULL);


	glDisableClientState(GL_VERTEX_ARRAY);
}
