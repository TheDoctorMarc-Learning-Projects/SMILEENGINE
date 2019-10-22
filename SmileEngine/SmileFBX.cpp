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
#include "ComponentTransform.h"
#include "ComponentMaterial.h"
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

void SmileFBX::ReadFBXData(const char* path)
{

	const aiScene* scene = aiImportFile(path, aiProcessPreset_TargetRealtime_MaxQuality);

	if (scene != nullptr && scene->HasMeshes()) 
	{
		// Create a GameObject with a "neutral" transform, same as root  
		ComponentTransform* transf = DBG_NEW ComponentTransform(); 
		GameObject* object = DBG_NEW GameObject(transf);

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
					mesh_info->minmaxCoords[minMaxCoords::MIN_X] = mesh_info->vertex[i];
					mesh_info->minmaxCoords[minMaxCoords::MAX_X] = mesh_info->vertex[i];
					mesh_info->minmaxCoords[minMaxCoords::MIN_Y] = mesh_info->vertex[i + 1];
					mesh_info->minmaxCoords[minMaxCoords::MAX_Y] = mesh_info->vertex[i + 1];
					mesh_info->minmaxCoords[minMaxCoords::MIN_Z] = mesh_info->vertex[i + 2];
					mesh_info->minmaxCoords[minMaxCoords::MAX_Z] = mesh_info->vertex[i + 2];
					continue; 
				}

				// find min-max X coord
				if (mesh_info->vertex[i] < mesh_info->minmaxCoords[minMaxCoords::MIN_X])
					mesh_info->minmaxCoords[minMaxCoords::MIN_X] = mesh_info->vertex[i]; 
				else if (mesh_info->vertex[i] > mesh_info->minmaxCoords[minMaxCoords::MAX_X])
					mesh_info->minmaxCoords[minMaxCoords::MAX_X] = mesh_info->vertex[i];

				// find min-max Y coord
				if (mesh_info->vertex[i + 1] < mesh_info->minmaxCoords[minMaxCoords::MIN_Y])
					mesh_info->minmaxCoords[minMaxCoords::MIN_Y] = mesh_info->vertex[i + 1];
				else if (mesh_info->vertex[i + 1] > mesh_info->minmaxCoords[minMaxCoords::MAX_Y])
					mesh_info->minmaxCoords[minMaxCoords::MAX_Y] = mesh_info->vertex[i + 1];

				// find min-max Z coord
				if (mesh_info->vertex[i + 2] < mesh_info->minmaxCoords[minMaxCoords::MIN_Z])
					mesh_info->minmaxCoords[minMaxCoords::MIN_Z] = mesh_info->vertex[i + 2];
				else if (mesh_info->vertex[i + 2] > mesh_info->minmaxCoords[minMaxCoords::MAX_Z])
					mesh_info->minmaxCoords[minMaxCoords::MAX_Z] = mesh_info->vertex[i + 2];

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
					memcpy(&mesh_info->color[j + 1], &new_mesh->mColors[0][i].g, sizeof(float));  
					memcpy(&mesh_info->color[j + 2], &new_mesh->mColors[0][i].b, sizeof(float));
					memcpy(&mesh_info->color[j + 3], &new_mesh->mColors[0][i].a, sizeof(float));
					j += 4;
				}
			}

			// create a component mesh with the mesh info  
			ComponentMesh* mesh = DBG_NEW ComponentMesh(mesh_info); 
			// Generate mesh buffers
			mesh->GenerateBuffers(); 
			// Asign it a transform as the mesh center (local matrix)
			ComponentTransform* transfMesh = DBG_NEW ComponentTransform(); 
			math::float4x4 transfMat = math::float4x4::identity; 
			transfMesh->ChangePosition(math::float3(mesh_info->GetMeshCenter().x, mesh_info->GetMeshCenter().y, mesh_info->GetMeshCenter().z));
			mesh->AddComponent(transfMesh); 

			// Texture last, once the mesh is created
			if (scene->HasMaterials())
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
				     	AssignTextureToMesh(assetsPath.c_str(), mesh);
					}
				}
			}
			
			// Fit the camera to the mesh 
			App->camera->FitCameraToMesh(mesh);

			// Assign a name to the mesh
			mesh->SetName(std::string(new_mesh->mName.C_Str()));  

			// Add the Mesh to the GameObject
			object->AddComponent(mesh);

		}

		char rawname[100]; 
		std::wcstombs(rawname, std::filesystem::path(path).stem().c_str(), sizeof(rawname));
		
		// Assign data to the object
		object->SetName(rawname);
		object->SetParent(App->scene_intro->rootObj);  
		object->Start(); 

		// Release the scene 
		aiReleaseImport(scene);
	}
	else
	{
		LOG("Error loading FBX %s", path);
	}
}

void SmileFBX::AssignTextureToMesh(const char* path, ComponentMesh* mesh)
{
	ComponentMaterial* previousMat = dynamic_cast<ComponentMaterial*>(std::get<Component*>(mesh->GetComponent(MATERIAL)));

	ILuint tempID;
	ilGenImages(1, &tempID);
	ilBindImage(tempID);

	ILboolean success = ilLoadImage(path);

	if ((bool)success)
	{
		// create a component material
		ComponentMaterial* targetMat = ((previousMat == nullptr) ? DBG_NEW ComponentMaterial() : previousMat);
		targetMat->CleanUpTextureData();

		iluFlipImage();
		ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE);

		glGenTextures(1, (GLuint*)&targetMat->textureInfo->id_texture);
		glBindTexture(GL_TEXTURE_2D, (GLuint)targetMat->textureInfo->id_texture);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		glTexImage2D(GL_TEXTURE_2D, 0, ilGetInteger(IL_IMAGE_FORMAT), (GLuint)ilGetInteger(IL_IMAGE_WIDTH),
			(GLuint)ilGetInteger(IL_IMAGE_WIDTH), 0, ilGetInteger(IL_IMAGE_FORMAT), GL_UNSIGNED_BYTE,
			ilGetData());

		glGenerateMipmap(GL_TEXTURE_2D);

		targetMat->textureInfo->width = (uint)ilGetInteger(IL_IMAGE_WIDTH);
		targetMat->textureInfo->height = (uint)ilGetInteger(IL_IMAGE_HEIGHT);
		targetMat->textureInfo->path = path;
		targetMat->textureInfo->texture = ilGetData();

		// Assign the material to the mesh
		if (targetMat != previousMat)
			mesh->AddComponent((Component*)targetMat);

	}

	glBindTexture(GL_TEXTURE_2D, 0);
	ilDeleteImage(tempID);



}


void SmileFBX::AssignCheckersTextureToMesh(ComponentMesh* mesh) // TODO: generic 
{
	ComponentMaterial* previousMat = dynamic_cast<ComponentMaterial*>(std::get<Component*>(mesh->GetComponent(MATERIAL)));

#ifndef CHECKERS_SIZE
#define CHECKERS_SIZE 20
#endif 

	// create a component material
	ComponentMaterial* targetMat = ((previousMat == nullptr) ? DBG_NEW ComponentMaterial() : previousMat);
	targetMat->CleanUpTextureData();

	// Generate the checkered image
	GLubyte checkImage[CHECKERS_SIZE][CHECKERS_SIZE][4];
	for (int i = 0; i < CHECKERS_SIZE; i++) 
	{
		for (int j = 0; j < CHECKERS_SIZE; j++)
		{
			int c = ((((i & 0x8) == 0) ^ (((j & 0x8)) == 0))) * 255;
			checkImage[i][j][0] = (GLubyte)c;
			checkImage[i][j][1] = (GLubyte)c;
			checkImage[i][j][2] = (GLubyte)c;
			checkImage[i][j][3] = (GLubyte)255;
		}
	}

	iluFlipImage();
	ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE);

	glGenTextures(1, (GLuint*)&targetMat->textureInfo->id_texture);
	glBindTexture(GL_TEXTURE_2D, (GLuint)targetMat->textureInfo->id_texture);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, (GLuint)CHECKERS_SIZE,
		(GLuint)CHECKERS_SIZE, 0, ilGetInteger(IL_IMAGE_FORMAT), GL_UNSIGNED_BYTE,
		ilGetData());

	targetMat->textureInfo->texture = (ILubyte*)ilGetData();

	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Assign the material to the mesh
	if (targetMat != previousMat)
		mesh->AddComponent((Component*)targetMat);

}


