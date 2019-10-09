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

void SmileFBX::ReadFBXData(const char* path) {

	const aiScene* scene = aiImportFile(path, aiProcessPreset_TargetRealtime_MaxQuality);

	if (scene != nullptr && scene->HasMeshes()) 
	{
		FBX fbx_info; 
		for (int i = 0; i < scene->mNumMeshes; ++i) 
		{

			aiMesh* new_mesh = scene->mMeshes[i];
			Mesh mesh_info; 
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

			if (new_mesh->HasNormals())
			{
				mesh_info.num_normals = new_mesh->mNumVertices;
				mesh_info.normals = new float[mesh_info.num_vertex * 3];
				memcpy(mesh_info.normals, new_mesh->mNormals, sizeof(float) * mesh_info.num_normals * 3);
			}


			glGenBuffers(1, (GLuint*) & (mesh_info.id_vertex));
			glBindBuffer(GL_ARRAY_BUFFER, mesh_info.id_vertex);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh_info.num_vertex * 3, mesh_info.vertex, GL_STATIC_DRAW);

			// Index buffer
			glGenBuffers(1, (GLuint*) & (mesh_info.id_index));
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh_info.id_index);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * mesh_info.num_index, mesh_info.index, GL_STATIC_DRAW);

			glGenBuffers(1, (GLuint*) & (mesh_info.id_normals));
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh_info.id_normals);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(float) * mesh_info.num_normals * 3, mesh_info.normals, GL_STATIC_DRAW);


			fbx_info.meshes.push_back(mesh_info); 
			App->scene_intro->fbxs.push_back(fbx_info);
		}
		// Vertex Buffer
		
		aiReleaseImport(scene);
	}
	else
	{
		LOG("Error loading FBX %s", path);
	}
}

void SmileFBX::DrawMesh(Mesh& mesh)
{
	glColor3f(0.3f, 0.3f, 0.3f);

	// Cient states
	glEnableClientState(GL_VERTEX_ARRAY); 
	glEnableClientState(GL_NORMAL_ARRAY);

	// TODO: normal buffer 
	if (mesh.normals != nullptr)
	{
		glBindBuffer(GL_NORMAL_ARRAY, mesh.id_normals);
		glNormalPointer(GL_FLOAT, 3, NULL);
	}

	// vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, mesh.id_vertex);
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	// index buffer 
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.id_index);
	glDrawElements(GL_TRIANGLES, mesh.num_index * 3, GL_UNSIGNED_INT, NULL);

	// Cient states
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);


	// draw normals
	if (mesh.normals != nullptr)
	{
		glColor3f(0.f, 1.0f, 0.f);
		static float normalFactor = 20.f;

		for (int i = 0; i < mesh.num_normals * 3; i += 3)
		{
			glBegin(GL_LINES);

			vec3 normalVec = normalize({ mesh.normals[i], mesh.normals[i + 1], mesh.normals[i + 2] });
			glVertex3f(mesh.vertex[i], mesh.vertex[i + 1], mesh.vertex[i + 2]);
			glVertex3f(mesh.vertex[i] + normalVec.x, mesh.vertex[i + 1] + normalVec.y, mesh.vertex[i + 2] + normalVec.z);

			glEnd();
		}
	}


}
