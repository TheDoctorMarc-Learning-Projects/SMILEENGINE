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
#include "ComponentMesh.h"
#include "ComponentMaterial.h"
#include <filesystem> // TODO: filesystem

#include <fstream>
#include "JSONParser.h"
#include "SmileUtilitiesModule.h"

SmileFBX::SmileFBX(SmileApp* app, bool start_enabled) : SmileModule(app, start_enabled) 
{

}

SmileFBX::~SmileFBX() 
{}

// ---------------------------------------------
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
	ilutInit(); 
	ilutRenderer(ILUT_OPENGL); 

	return ret;
}

// ---------------------------------------------
bool SmileFBX::CleanUp()
{
	aiDetachAllLogStreams();
	return true;
}

// ---------------------------------------------

void SmileFBX::Load(const char* path, std::string extension)
{
	if (IsModelExtension(extension) == true)
	{
		if (IsOwnModelExtension(extension) == false) // 1) If FBX not in folder, push it to folder. 2) If .model does not exist, generate it
			LoadFBX(path);
		else
			LoadModel(path); 

    }
}

// ---------------------------------------------
#define ObjectBegin {
#define ObjectEnd }

const aiScene* OnFBXImport(const char* path, char* rawname)
{
	strcpy(rawname, std::filesystem::path(path).stem().string().c_str());
	return aiImportFile(path, aiProcessPreset_TargetRealtime_Fast);
}

static void OnFBXImportEnd(GameObject* parentObj, const aiScene* scene)
{
 
	aiReleaseImport(scene);
	App->object_manager->DestroyObject(parentObj); 
}

GameObject* SmileFBX::LoadFBX(const char* path)
{
	
	char rawname[100];
	const aiScene* scene = OnFBXImport(path, rawname);

	bool success = scene && scene->HasMeshes(); 
 	
	if (!success)
	{
		LOG("Error loading FBX %s", path);
		return nullptr;
	}


	// 1) If FBX not in folder, push it to folder
	if (DoesFBXExistInAssets(path) == false)
		path = PushFBXToAssets(path);

	// 2) If.model does not exist, generate it
	if (DoesFBXHaveLinkedModel(path) == false)
		GenerateModelFromFBX(path, scene, rawname);
	 
}

bool SmileFBX::DoesFBXExistInAssets(const char* path)
{
	std::string cleanPath[1]; 
	std::string file[1];
	std::string extension[1];
	App->fs->SplitFilePath(path, cleanPath, file, extension);

	fbx_target = std::string(std::string(ASSETS_MODELS_FOLDER) + file[0]);


	if (App->fs->Exists(fbx_target.c_str()))
	{
		return true;
	}
	
	
	return false;
	
}

const char* SmileFBX::PushFBXToAssets(const char* path)
{
	App->fs->CopyFromOutsideFS(path, fbx_target.c_str());

	return path; 
}

bool SmileFBX::DoesFBXHaveLinkedModel(const char* path)
{
	std::string cleanPath[1];
	std::string file[1];
	std::string extension[1];
	App->fs->SplitFilePath(path, cleanPath, file, extension);

	models_target = std::string(std::string(LIBRARY_MODELS_FOLDER) + file[0]);
	models_target = models_target.substr(0, models_target.find_last_of(".") + 1);
	models_target = models_target.substr(models_target.find_last_of("/") + 1, models_target.find_last_of("."));
	models_target = models_target + std::string(MODELS_EXTENSION);

	if (App->fs->Exists(models_target.c_str()))
	{
		return true;
	}

	return false;
}


GameObject* SmileFBX::GenerateModelFromFBX(const char* path, const aiScene* scene, char* rawname)
{

	// Parent Object
	ComponentTransform* transf = DBG_NEW ComponentTransform(math::float4x4::identity);
	GameObject* parentObj = DBG_NEW GameObject(transf, rawname, App->scene_intro->rootObj);

	for (int i = 0; i < scene->mNumMeshes; ++i)

	ObjectBegin

		// Mesh
	ModelMeshData* mesh_info = FillMeshBuffers(scene->mMeshes[i], DBG_NEW ModelMeshData());
	ComponentMesh* mesh = DBG_NEW ComponentMesh(mesh_info, "Mesh");

	// Materials
	std::vector<std::string> materialsPaths = ReadFBXMaterials(scene);

	// Child Object
	ResolveObjectFromFBX(DBG_NEW GameObject(std::string(scene->mMeshes[i]->mName.C_Str()), parentObj),
		mesh, materialsPaths);

	ObjectEnd
		
	SaveModel(parentObj, path);
	OnFBXImportEnd(parentObj, scene);

	return parentObj;
}


// ---------------------------------------------
void SmileFBX::ResolveObjectFromFBX(GameObject* object, ComponentMesh* mesh, std::vector<std::string> materialsPaths, const char* path)
{
	/// 1) Create and Assign components
	// Mesh
	object->AddComponent(mesh);
	object->SetupTransformAtMeshCenter();

	// Materials
	for (auto& path : materialsPaths)
	{
		AssignTextureToObj(path.c_str(), object);
		LOG("Asset loaded: %s", path.c_str());
	}
}

// ---------------------------------------------
std::vector<std::string> SmileFBX::ReadFBXMaterials(const aiScene* scene)
{
	std::vector<std::string> materialsPaths; 

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

				materialsPaths.push_back(std::string("Assets/Models/") + std::string(tex_path.data));

			}

		}
	}

	return materialsPaths; 
}

// ---------------------------------------------
ModelMeshData* SmileFBX::FillMeshBuffers(aiMesh* new_mesh, ModelMeshData* mesh_info)
{
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
		for (uint i = 0; i < new_mesh->mNumFaces; ++i)
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

	// This is replaced in the other branch I believe: 
	mesh_info->ComputeMeshSpatialData();

	return mesh_info; 
}

// ---------------------------------------------
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
			(GLuint)ilGetInteger(IL_IMAGE_HEIGHT), 0, ilGetInteger(IL_IMAGE_FORMAT), GL_UNSIGNED_BYTE,
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

		if (targetMat->textureInfo->format == "RGBA")
			textInfo.rgba--;

		targetMat->textureInfo->format = "RGB";

	}
	else
		LOG("Error trying to load a texture image :( %s", iluErrorString(ilGetError()));

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

ComponentMesh* SmileFBX::LoadMesh(ModelMeshData* mesh, const char* full_path)
{
	
	char* buffer = nullptr;
	App->fs->Load(full_path, &buffer);

	char* cursor = buffer;
	uint ranges[4];
	uint bytes = sizeof(ranges);
	memcpy(ranges, cursor, bytes);
	mesh->num_index = ranges[0];
	mesh->num_vertex = ranges[1];
	mesh->num_normals = ranges[2];
	mesh->num_UVs = ranges[3];

	// Load indices
	cursor += bytes;
	bytes = sizeof(uint) * mesh->num_index;
	mesh->index = new uint[mesh->num_index];
	memcpy(mesh->index, cursor, bytes);

	//Load vertex
	cursor += bytes;
	bytes = sizeof(float) * mesh->num_vertex * 3;
	mesh->vertex = new float[mesh->num_vertex * 3];
	memcpy(mesh->vertex, cursor, bytes);

	//Load normals
	cursor += bytes;
	bytes = sizeof(float) * mesh->num_normals * 3;
	mesh->normals = new float[mesh->num_normals * 3];
	memcpy(mesh->normals, cursor, bytes);

	//Load UVs
	cursor += bytes;
	bytes = sizeof(float) * mesh->num_UVs * 2;
	mesh->UVs = new float[mesh->num_UVs * 2];
	memcpy(mesh->UVs, cursor, bytes);

	std::string cleanPath[1];
	std::string file[1];
	std::string extension[1];

	App->fs->SplitFilePath(full_path, cleanPath, file, extension);
	ComponentMesh* component_mesh = DBG_NEW ComponentMesh(mesh,file[0]);
	LOG("Loading mesh: %s", full_path);
	
	return component_mesh;
}

std::string SmileFBX::SaveMesh(ModelMeshData* mesh, GameObject* obj, uint index)
{
	
	bool ret = false;
	uint ranges[4] = { mesh->num_index, mesh->num_vertex, mesh->num_normals, mesh->num_UVs };
	uint size = sizeof(ranges) + sizeof(uint) * mesh->num_index + sizeof(float) * mesh->num_vertex * 3 + sizeof(float) * mesh->num_normals * 3 + sizeof(float) * mesh->num_UVs * 2;
	char* data = new char[size]; // Allocate
	char* cursor = data;

	uint bytes = sizeof(ranges); // First store ranges
	memcpy(cursor, ranges, bytes);

	//index
	cursor += bytes; // Store indices
	bytes = sizeof(uint) * mesh->num_index;
	memcpy(cursor, mesh->index, bytes);

	//vertex
	cursor += bytes;
	bytes = sizeof(float) * mesh->num_vertex * 3;
	memcpy(cursor, mesh->vertex, bytes);

	//normals
	cursor += bytes;
	bytes = sizeof(float) * mesh->num_normals * 3;
	memcpy(cursor, mesh->normals, bytes);

	//uvs
	cursor += bytes;
	bytes = sizeof(float) * mesh->num_UVs * 2;
	memcpy(cursor, mesh->UVs, bytes);

	std::string name;
	std::string output; 
	name += std::string(obj->GetName().c_str()) += std::string("_mesh") += std::to_string(index);   
	
	App->fs->SaveUnique(output, data, size, LIBRARY_MESHES_FOLDER, name.c_str(), MESH_EXTENSION);

	RELEASE_ARRAY(data);

	return output;
}


std::string SmileFBX::SaveMaterial(const char* path)
{
	ILuint size;
	ILubyte* data;
	std::string fullName(path); 
	std::string output_file, cleanPath, extension, fileName; 
	App->fs->SplitFilePath(path, &output_file, &fileName); 
	std::string rawname = fileName.substr(0, fileName.find_last_of("."));

	ilSetInteger(IL_DXTC_FORMAT, IL_DXT5);
	size = ilSaveL(IL_DDS, NULL, 0);
	if (size > 0) {
		data = DBG_NEW ILubyte[size];
		if (ilSaveL(IL_DDS, data, size) > 0)
			App->fs->SaveUnique(output_file, data, size, LIBRARY_TEXTURES_FOLDER, rawname.c_str(), "dds");

		RELEASE_ARRAY(data); 
	}
	

	std::string library(LIBRARY_TEXTURES_FOLDER);
	App->fs->EraseChunckFromString(library, 0, 1); 
	return output_file = library + rawname + std::string(".dds");
}

bool SmileFBX::LoadModel(const char* path)
{
	rapidjson::Document doc;
	dynamic_cast<JSONParser*>(App->utilities->GetUtility("JSONParser"))->ParseJSONFile(path, doc);
	/*int id = rapidjson::GetValueByPointer(doc, "/GameObject/0/ID")->GetInt();
	int parent_id = rapidjson::GetValueByPointer(doc, "/GameObject/0/Parent ID")->GetInt();*/
	//bool selected = rapidjson::GetValueByPointer(doc, "/GameObject/0/Selected")->GetBool();
	std::string name = rapidjson::GetValueByPointer(doc, "/GameObject/0/Name")->GetString();
	std::string fbx_path = rapidjson::GetValueByPointer(doc, "/GameObject/0/FBX path")->GetString();

	char rawname[100];
	strcpy(rawname, std::filesystem::path(path).stem().string().c_str());
	ComponentTransform* transf = DBG_NEW ComponentTransform(math::float4x4::identity);
	GameObject* parentObj = DBG_NEW GameObject(transf, rawname, App->scene_intro->rootObj);

	rapidjson::Value& a = doc["Meshes"];
 
	// Read Mesh paths: 
	for (rapidjson::SizeType i = 0; i < a.Size(); i++)
	{
		std::string path = a[i]["path"].GetString(); 
		std::string materialPath = a[i]["materialPath"].GetString();

		LOG("Loading a mesh and maybe a material!"); 
		ModelMeshData* mesh = DBG_NEW ModelMeshData;
		textureData* texdata = DBG_NEW textureData;

		GameObject* child = DBG_NEW GameObject;

		child->AddComponent(LoadMesh(mesh,path.c_str()));
		if (materialPath != "empty")
			AssignTextureToObj(materialPath.c_str(), child); 

		child->SetParent(parentObj);
		
	}

	parentObj->Start();

	return true;
}

void SmileFBX::SaveModel(GameObject* obj, const char* path)
{
	// 1) Save components (at the same time when saving the model below)
	
	auto transf = obj->GetTransform();
	auto material = obj->GetMaterial();   
	std::vector <GameObject*>children = obj->GetImmidiateChildren();

	float3 position = transf->GetPosition();
	float3 scale = transf->GetScale();
	Quat rotation = transf->GetRotation();

	// 2) Save the object itself
	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);


	// window 
	writer.StartObject();
	writer.Key("GameObject");

	writer.StartArray();

	writer.StartObject();

	// Model variables
	writer.Key("ID");
	writer.Int(obj->GetID());

	GameObject* parent = obj->GetParent(); 
	if (parent)
	{
		writer.Key("Parent ID");
		writer.Int(parent->GetID());
	}

	writer.Key("Name");
	writer.String(obj->GetName().c_str());

	writer.Key("Selected");
	writer.Bool((App->scene_intro->selectedObj == obj) ? true : false);

	std::string lastFBXPath = path;
	
	writer.Key("FBX path");
	writer.String(lastFBXPath.c_str()); // TODO: FILL THE LASTFBXPATH

	// TODO: active, static, open in hierarchy , AABB, OBB 

	// Components


		
	/*if (material)
	{
		writer.Key("Material path");
		writer.String(SaveMaterial(material->GetTextureData()).c_str());
	}*/


	// Components

	writer.EndObject();

	writer.EndArray();

	writer.Key("Meshes");

	writer.StartArray();

	

	uint meshCount = 0; 
	for (auto& child : children)
	{
		if (child->GetMesh())
		{
			writer.StartObject();
			meshCount++; 

			auto material = child->GetMaterial(); 
			auto mesh = child->GetMesh();
			

			writer.Key("path");
			writer.String(SaveMesh(mesh->GetMeshData(), child, meshCount).c_str());

			writer.Key("materialPath");

			if(material)
				writer.String(SaveMaterial(material->GetTextureData()->path.c_str()).c_str());
			else
				writer.String("empty");
			
			writer.EndObject();
			
		}
	}

	

	writer.EndArray();

	writer.EndObject();
	
	const char* output = buffer.GetString();
	std::string dirPath; 
	App->fs->SaveUnique(dirPath, output, buffer.GetSize(), LIBRARY_MODELS_FOLDER, obj->GetName().c_str(), "json");

 
}


