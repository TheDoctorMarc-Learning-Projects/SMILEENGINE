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

	LOG("Initializing Devil");
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

	const aiScene* scene = aiImportFile(path, aiProcessPreset_TargetRealtime_Fast);

	// Get the parent GameObject name
	char rawname[100];
	std::wcstombs(rawname, std::filesystem::path(path).stem().c_str(), sizeof(rawname));

	if (scene != nullptr && scene->HasMeshes()) 
	{
		// Create a GameObject with a "neutral" transform, same as root  
		ComponentTransform* transf = DBG_NEW ComponentTransform(math::float4x4::identity); 
		GameObject* parentObj = DBG_NEW GameObject(transf, rawname, App->scene_intro->rootObj);

		for (int i = 0; i < scene->mNumMeshes; ++i) 
		{
			// Create a mesh data and an object, child of the first one
			aiMesh* new_mesh = scene->mMeshes[i];
			GameObject* object = DBG_NEW GameObject(std::string(new_mesh->mName.C_Str()), parentObj);
			ModelMeshData* mesh_info = DBG_NEW ModelMeshData();

			// Vertexs
			mesh_info->num_vertex = new_mesh->mNumVertices;
			mesh_info->vertex = new float[mesh_info->num_vertex * 3];
			memcpy(mesh_info->vertex, new_mesh->mVertices, sizeof(float) * mesh_info->num_vertex * 3);
			LOG("New Mesh with %d vertices", mesh_info->num_vertex);

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
			LOG("UVs: %f", mesh_info->UVs);

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
				LOG("Number of vertices: %i", new_mesh->mNumVertices);
			}

			// create a component mesh and fill it with the mesh info
			mesh_info->ComputeMeshSpatialData();
			ComponentMesh* mesh = DBG_NEW ComponentMesh(mesh_info, "Mesh");

			// Assign a texture to the object
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

						std::string assetsPath("Assets/Models/"); assetsPath += tex_path.data;
						AssignTextureToObj(assetsPath.c_str(), object);
						LOG("Asset loaded: %s", assetsPath.c_str());
					}
				}
			}
			
			// Add the Mesh to the GameObject and the GameObject to the parent GameObject
			object->AddComponent(mesh);
			object->SetupTransformAtMeshCenter(); 

			// Fit the camera to the object 
			App->camera->FitCameraToObject(object);

		}
		
		// Start everything
		parentObj->Start();

		// Release the scene 
		aiReleaseImport(scene);
	}
	else
	{
		LOG("Error loading FBX %s", path);
	}
}

void SmileFBX::AssignTextureToObj(const char* path, GameObject* obj)
{
	ComponentMaterial* previousMat = dynamic_cast<ComponentMaterial*>(obj->GetComponent(MATERIAL));

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
		{
			obj->AddComponent((Component*)targetMat);
			textInfo.totalActiveTextures++;
		}
			
		textInfo.rgb++;

		if(targetMat->textureInfo->format == "RGBA")
			textInfo.rgba--;

		targetMat->textureInfo->format = "RGB";

	}

	glBindTexture(GL_TEXTURE_2D, 0);
	ilDeleteImage(tempID);



}


void SmileFBX::AssignCheckersTextureToObj(GameObject* obj) // TODO: generic 
{
	ComponentMaterial* previousMat = dynamic_cast<ComponentMaterial*>(obj->GetComponent(MATERIAL));

#ifndef CHECKERS_SIZE
#define CHECKERS_SIZE 20
#endif 

	// create a component material
	ComponentMaterial* targetMat = ((previousMat == nullptr) ? DBG_NEW ComponentMaterial() : previousMat);
	targetMat->CleanUpTextureData();

	// Generate the checkered image
	GLubyte checkImage[CHECKERS_SIZE][CHECKERS_SIZE][4];
	for (uint i = 0; i < CHECKERS_SIZE; i++) 
	{
		for (uint j = 0; j < CHECKERS_SIZE; j++)
		{
			uint c = ((((i & 0x8) == 0) ^ (((j & 0x8)) == 0))) * 255;
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

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (GLuint)CHECKERS_SIZE,
		(GLuint)CHECKERS_SIZE, 0, GL_RGBA, GL_UNSIGNED_BYTE,
		checkImage);

	targetMat->textureInfo->texture = (ILubyte*)checkImage;
	targetMat->textureInfo->path = "Generated in-game"; 

	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Assign the material to the mesh
	if (targetMat != previousMat)
	{
		obj->AddComponent((Component*)targetMat);
		textInfo.totalActiveTextures++;
	}
		
	textInfo.rgba++;

	if (targetMat->textureInfo->format == "RGB")
		textInfo.rgb--;

	targetMat->textureInfo->format = "RGBA";

}

bool SmileFBX::SaveMesh(ModelMeshData* mesh)
{
	bool ret = false;
	uint ranges[2] = { mesh->num_index, mesh->num_vertex };
	uint size = sizeof(ranges) + sizeof(uint) * mesh->num_index + sizeof(float) * mesh->num_vertex * 3;
	char* data = new char[size]; // Allocate
	char* cursor = data;
	uint bytes = sizeof(ranges); // First store ranges
	memcpy(cursor, ranges, bytes);
	cursor += bytes; // Store indices
	bytes = sizeof(uint) * mesh->num_index;
	memcpy(cursor, mesh->index, bytes);

	return ret;
}


