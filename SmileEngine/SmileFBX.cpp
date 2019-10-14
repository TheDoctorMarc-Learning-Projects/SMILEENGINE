#include "SmileApp.h"
#include "SmileFBX.h"
#include "Glew/include/GL/glew.h" 
#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"
#pragma comment (lib, "Assimp/libx86/assimp.lib")

#include "DevIL/include/IL/ilu.h"
#include "DevIL/include/IL/ilut.h"

#pragma comment (lib, "DevIL/libx86/DevIL.lib")
#pragma comment (lib, "DevIL/libx86/ILU.lib")
#pragma comment (lib, "DevIL/libx86/ILUT.lib")

#include "GameObject.h"
#include <filesystem> // TODO: filesystem

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

	// Devil
	ilInit(); 
	iluInit(); 
	ilutRenderer(ILUT_OPENGL); 

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
		GameObject* object = DBG_NEW GameObject();

		for (int i = 0; i < scene->mNumMeshes; ++i) 
		{
			aiMesh* new_mesh = scene->mMeshes[i];
			ModelMeshData* mesh_info = DBG_NEW ModelMeshData();

			// Vertexs
			mesh_info->num_vertex = new_mesh->mNumVertices;
			mesh_info->vertex = new float[mesh_info->num_vertex * 3];
			memcpy(mesh_info->vertex, new_mesh->mVertices, sizeof(float) * mesh_info->num_vertex * 3);
			LOG("New Mesh with %d vertices", mesh_info->num_vertex);

			// Get the minimum and maximum x,y,z to create a bounding box 
			for (uint i = 0; i < mesh_info->num_vertex; i+=3) 
			{
				// first, initialize the min-max coords to the first vertex, 
				// in order to compare the following ones with it
				if (i == 0)
				{
					mesh_info->minmaxCoords[mesh_info->minMaxCoords::MIN_X] = mesh_info->vertex[i];
					mesh_info->minmaxCoords[mesh_info->minMaxCoords::MAX_X] = mesh_info->vertex[i];
					mesh_info->minmaxCoords[mesh_info->minMaxCoords::MIN_Y] = mesh_info->vertex[i + 1];
					mesh_info->minmaxCoords[mesh_info->minMaxCoords::MAX_Y] = mesh_info->vertex[i + 1];
					mesh_info->minmaxCoords[mesh_info->minMaxCoords::MIN_Z] = mesh_info->vertex[i + 2];
					mesh_info->minmaxCoords[mesh_info->minMaxCoords::MAX_Z] = mesh_info->vertex[i + 2];
					continue; 
				}

				// find min-max X coord
				if (mesh_info->vertex[i] < mesh_info->minmaxCoords[mesh_info->minMaxCoords::MIN_X])
					mesh_info->minmaxCoords[mesh_info->minMaxCoords::MIN_X] = mesh_info->vertex[i]; 
				else if (mesh_info->vertex[i] > mesh_info->minmaxCoords[mesh_info->minMaxCoords::MAX_X])
					mesh_info->minmaxCoords[mesh_info->minMaxCoords::MAX_X] = mesh_info->vertex[i];

				// find min-max Y coord
				if (mesh_info->vertex[i + 1] < mesh_info->minmaxCoords[mesh_info->minMaxCoords::MIN_Y])
					mesh_info->minmaxCoords[mesh_info->minMaxCoords::MIN_Y] = mesh_info->vertex[i + 1];
				else if (mesh_info->vertex[i + 1] > mesh_info->minmaxCoords[mesh_info->minMaxCoords::MAX_Y])
					mesh_info->minmaxCoords[mesh_info->minMaxCoords::MAX_Y] = mesh_info->vertex[i + 1];

				// find min-max Z coord
				if (mesh_info->vertex[i + 2] < mesh_info->minmaxCoords[mesh_info->minMaxCoords::MIN_Z])
					mesh_info->minmaxCoords[mesh_info->minMaxCoords::MIN_Z] = mesh_info->vertex[i + 2];
				else if (mesh_info->vertex[i + 2] > mesh_info->minmaxCoords[mesh_info->minMaxCoords::MAX_Z])
					mesh_info->minmaxCoords[mesh_info->minMaxCoords::MAX_Z] = mesh_info->vertex[i + 2];

			}

			mesh_info->ComputeMeshSpatialData(); 

		    // Indexes
			if (new_mesh->HasFaces())
			{
				mesh_info->num_index = new_mesh->mNumFaces * 3;
				mesh_info->index = new uint[mesh_info->num_index];
				for(uint i = 0; i< new_mesh->mNumFaces; ++i)
				{
					if (new_mesh->mFaces[i].mNumIndices != 3)
					{
						LOG("WARNING, geometry face with != 3 indices!");
						memset(&mesh_info->index[i * 3], 0, sizeof(uint) * 3);
					}
					else
					{
						memcpy(&mesh_info->index[i * 3], &new_mesh->mFaces[i].mIndices[0], sizeof(uint) * 3);
					}
					
				}
			
			}

			// Normals
			if (new_mesh->HasNormals())
			{
				mesh_info->num_normals = new_mesh->mNumVertices;
				mesh_info->normals = new float[mesh_info->num_vertex * 3];
				memcpy(mesh_info->normals, new_mesh->mNormals, sizeof(float) * mesh_info->num_normals * 3);

			}

			// UVs
			for (int ind = 0; ind < new_mesh->GetNumUVChannels(); ++ind)
			{
				if (new_mesh->HasTextureCoords(ind))
				{
					mesh_info->num_UVs = new_mesh->mNumVertices;
					mesh_info->UVs = new float[mesh_info->num_UVs * 2];

					uint j = 0;
					for (uint i = 0; i < new_mesh->mNumVertices; ++i) 
					{

						//there are two for each vertex
						memcpy(&mesh_info->UVs[j], &new_mesh->mTextureCoords[ind][i].x, sizeof(float));
						memcpy(&mesh_info->UVs[j + 1], &new_mesh->mTextureCoords[ind][i].y, sizeof(float));
						j += 2;
					}
			 
				}

			}

			// Colors 
			if (new_mesh->HasVertexColors(0))  
			{
				mesh_info->num_color = new_mesh->mNumVertices;
				mesh_info->color = new float[mesh_info->num_color * 4];
				uint j = 0;
				for (uint i = 0; i < new_mesh->mNumVertices; ++i)
				{
					memcpy(&mesh_info->color[j], &new_mesh->mColors[0][i].r, sizeof(float));
					memcpy(&mesh_info->color[j + 1], &new_mesh->mColors[0][i].g, sizeof(float)); //row var needed
					memcpy(&mesh_info->color[j + 2], &new_mesh->mColors[0][i].b, sizeof(float));
					memcpy(&mesh_info->color[j + 3], &new_mesh->mColors[0][i].a, sizeof(float));
					j += 4;
				}
			}


			// Normals Buffer
			glGenBuffers(1, (GLuint*) & (mesh_info->id_normals));
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh_info->id_normals);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(float) * mesh_info->num_normals * 3, mesh_info->normals, GL_STATIC_DRAW);
			
			// Uvs vBuffer
			glGenBuffers(1, (GLuint*) & (mesh_info->id_UVs));
			glBindBuffer(GL_ARRAY_BUFFER, mesh_info->id_UVs);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh_info->num_UVs * 2, mesh_info->UVs, GL_STATIC_DRAW);

			// Color Buffer
			glGenBuffers(1, (GLuint*) & (mesh_info->id_color));
			glBindBuffer(GL_ARRAY_BUFFER, mesh_info->id_color);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh_info->num_color * 3, mesh_info->color, GL_STATIC_DRAW);
		
			// Vertex Buffer
			glGenBuffers(1, (GLuint*) & (mesh_info->id_vertex));
			glBindBuffer(GL_ARRAY_BUFFER, mesh_info->id_vertex);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh_info->num_vertex * 3, mesh_info->vertex, GL_STATIC_DRAW);

			// Index Buffer
			glGenBuffers(1, (GLuint*) & (mesh_info->id_index));
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh_info->id_index);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * mesh_info->num_index, mesh_info->index, GL_STATIC_DRAW);

			// create a component mesh with the mesh info and then add it to the gameobject
			ComponentMesh* mesh = DBG_NEW ComponentMesh(mesh_info); 
			object->AddComponent(mesh);
			App->camera->FitMeshToCamera(mesh);


			// Texture last, once the mesh is created
			/*if (scene->HasMaterials())
			{
				for (unsigned int i = 0; i < scene->mNumMaterials; ++i)
				{
					const aiMaterial* material = scene->mMaterials[i];
					uint nTex = material->GetTextureCount(aiTextureType_DIFFUSE);

					for (uint i = 0; i < nTex; ++i)
					{
						aiString tex_path;
						scene->mMaterials[i]->GetTexture(aiTextureType_DIFFUSE, i, &tex_path);

						std::string assetsPath("..//Assets/"); assetsPath += tex_path.data; 
						App->object_manager->AssignTextureImageToMesh(assetsPath.c_str(), mesh);
					}
				}
			}*/
			

		}

		App->scene_intro->objects.push_back(object); 
		aiReleaseImport(scene);
	}
	else
	{
		LOG("Error loading FBX %s", path);
	}
}

