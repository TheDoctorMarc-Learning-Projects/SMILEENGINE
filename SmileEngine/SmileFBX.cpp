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

#include <fstream>
#include "JSONParser.h"
#include "SmileUtilitiesModule.h"

#include "ResourceMesh.h"
#include "Resource.h"
#include "ResourceTexture.h"
#include <filesystem>

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

const aiScene* OnFBXImport(const char* path, char* rawname)
{
	strcpy(rawname, std::filesystem::path(path).stem().string().c_str());
	return aiImportFile(path, aiProcessPreset_TargetRealtime_MaxQuality);
}

static void OnFBXImportEnd(GameObject* parentObj, const aiScene* scene)
{
 
	aiReleaseImport(scene);
	App->object_manager->DestroyObject(parentObj); 
}

void SmileFBX::LoadFBX(const char* path)
{
	GameObject* ret; 
	char rawname[100];
	const aiScene* scene = OnFBXImport(path, rawname);
	bool success = scene && scene->HasMeshes(); 

if (!success)
{
	LOG("Error loading FBX %s", path);
	return;
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

	fbx_target = cleanPath[0] + file[0];


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


void SmileFBX::GenerateModelFromFBX(const char* path, const aiScene* scene, char* rawname)
{
	lastFbxPath = path;
	lastFbxFolder = App->fs->GetDirectoryFromPath(path); 

	// Parent Object
	ComponentTransform* transf = DBG_NEW ComponentTransform(math::float4x4::identity);
	fbxParent = App->object_manager->CreateGameObject(transf, rawname, App->scene_intro->rootObj);

	// Load nodes recursively 
	uint index = 0; 
	LoadFBXnode(scene->mRootNode, scene);

	// Get parent object transform and start everything
	aiVector3D position, scale;
	aiQuaternion rot;
	scene->mRootNode->mTransformation.Decompose(scale, rot, position);
	transf->SetupTransform(float4x4::FromTRS(float3(position.x, position.y, position.z),
		Quat(rot.x, rot.y, rot.z, rot.w), float3(scale.x, scale.y, scale.z)));

	// Save parent
	SaveModel(fbxParent, path);

	// Destroy obj created from fbx and scene
	OnFBXImportEnd(fbxParent, scene);
}


void SmileFBX::LoadFBXnode(aiNode* node, const aiScene* scene)
{
	
	for (int i = 0; i < node->mNumMeshes; ++i)
	{

		aiMesh* aiMesh = scene->mMeshes[node->mMeshes[i]]; 

		// Mesh
		ModelMeshData* mesh_info = FillMeshBuffers(aiMesh, DBG_NEW ModelMeshData());
		ResourceMesh* resMesh = dynamic_cast<ResourceMesh*>(App->resources->CreateNewResource(RESOURCE_MESH, "empty"));
		resMesh->model_mesh = mesh_info;
		ComponentMesh* mesh = DBG_NEW ComponentMesh(resMesh->GetUID(), "Mesh");

		// Materials
		aiMaterial* material = scene->mMaterials[aiMesh->mMaterialIndex];
		aiString fileName;
		material->GetTexture(aiTextureType_DIFFUSE, 0, &fileName);
		std::string matPath = "empty"; 
		if (strcmp(fileName.C_Str(), "") != 0)
		{
			// get the file (nane  extension) alone, without anything on the left
			std::string realFile, path;
			App->fs->SplitFilePath(fileName.C_Str(), &path, &realFile);
			matPath = lastFbxFolder + realFile.c_str();

		}
		
		// Capture pos, rot, scale
		aiVector3D position, scale;
		aiQuaternion rot;
		node->mTransformation.Decompose(scale, rot, position);
	 
		ComponentTransform* transf = DBG_NEW ComponentTransform(); 
		transf->SetupTransform(float4x4::FromTRS(float3(position.x, position.y, position.z), 
			Quat(rot.x, rot.y, rot.z, rot.w), float3(scale.x, scale.y, scale.z)));

		// Child Object -> add mesh, material, transform
		std::vector<Component*> comps;
		comps.push_back(transf); 
		comps.push_back(mesh); 

		GameObject* childObj = App->object_manager->CreateGameObject(comps, node->mName.C_Str(), fbxParent);
	
		if(matPath != "empty")
			AssignTextureToObj(matPath.c_str(), childObj);

	}

	for (uint i = 0; i < node->mNumChildren; i++)
	{
		LoadFBXnode(node->mChildren[i], scene);
	}

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


	return mesh_info; 
}

// ---------------------------------------------
void SmileFBX::AssignTextureToObj(const char* path, GameObject* obj)
{
	std::string full_path, cleanPath, file; 
	App->fs->SplitFilePath(path, &full_path, &cleanPath, &file);

	ResourceTexture* res = (ResourceTexture*)App->resources->GetResourceByPath(path);

	if (res)
		obj->AddComponent((Component*)DBG_NEW ComponentMaterial(res->GetUID(), file.c_str()));
	else
	{
		res = (ResourceTexture*)App->resources->CreateNewResource(RESOURCE_TEXTURE, path);
		res->LoadOnMemory(path);
		obj->AddComponent((Component*)DBG_NEW ComponentMaterial(res->GetUID(), "Material"));
	}

}


void SmileFBX::AssignCheckersTextureToObj(GameObject* obj) // TODO: generic 
{
	ResourceTexture* res = App->resources->checkersTexture;
	obj->AddComponent((Component*)DBG_NEW ComponentMaterial(res->GetUID(), "Checkers"));

}

ComponentMesh* SmileFBX::LoadMesh(const char* full_path) // should create a resource mesh, or not, if it exists!
{
	std::string cleanPath[1];
	std::string file[1];
	std::string extension[1];

	App->fs->SplitFilePath(full_path, cleanPath, file, extension);

	Resource* res = App->resources->GetResourceByPath(full_path);

	if (res) return DBG_NEW ComponentMesh(res->GetUID(), file[0]);


	ModelMeshData* mesh = DBG_NEW ModelMeshData;
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

	ResourceMesh* resmesh = dynamic_cast<ResourceMesh*>(App->resources->CreateNewResource(RESOURCE_MESH, full_path));
	resmesh->model_mesh = mesh;
	resmesh->LoadOnMemory(); 
	
	LOG("Loading mesh: %s", full_path);
	
	return DBG_NEW ComponentMesh(resmesh->GetUID(), file[0]);
}

// Shold save a resource mesh, or not, if id does already exist
std::string SmileFBX::SaveMesh(ResourceMesh* resource, GameObject* obj, uint index)
{
	auto bufferData = resource->model_mesh; 
	bool ret = false;
	uint ranges[4] = { bufferData->num_index, bufferData->num_vertex, bufferData->num_normals, bufferData->num_UVs };
	uint size = sizeof(ranges) + sizeof(uint) * bufferData->num_index + sizeof(float) * bufferData->num_vertex * 3 + sizeof(float) * bufferData->num_normals * 3 + sizeof(float) * bufferData->num_UVs * 2;
	char* data = new char[size]; // Allocate
	char* cursor = data;

	uint bytes = sizeof(ranges); // First store ranges
	memcpy(cursor, ranges, bytes);

	//index
	cursor += bytes; // Store indices
	bytes = sizeof(uint) * bufferData->num_index;
	memcpy(cursor, bufferData->index, bytes);

	//vertex
	cursor += bytes;
	bytes = sizeof(float) * bufferData->num_vertex * 3;
	memcpy(cursor, bufferData->vertex, bytes);

	//normals
	cursor += bytes;
	bytes = sizeof(float) * bufferData->num_normals * 3;
	memcpy(cursor, bufferData->normals, bytes);

	//uvs
	cursor += bytes;
	bytes = sizeof(float) * bufferData->num_UVs * 2;
	memcpy(cursor, bufferData->UVs, bytes);

	std::string name;
	std::string output; 
	name += std::string(obj->GetName().c_str()) += std::string("_mesh");
	if (index != INT_MAX)
		name += std::to_string(index);  
		
	App->fs->SaveUnique(output, data, size, LIBRARY_MESHES_FOLDER, name.c_str(), MESH_EXTENSION);

	RELEASE_ARRAY(data);

	return output;
}


std::string SmileFBX::SaveMaterial(const char* path)
{
	const std::filesystem::path p = std::filesystem::path(path);
	std::filesystem::path rel = std::filesystem::relative(p);
	std::string realPath(rel.string().c_str()); 
	std::replace(realPath.begin(), realPath.end(), '\\', '/');
 
	if (App->fs->Exists(realPath.c_str()) == false)
		return "no material found"; 

	ILuint size;
	ILubyte* data;
	std::string output_file, cleanPath, extension, fileName; 
	App->fs->SplitFilePath(realPath.c_str(), &output_file, &fileName);
	std::string rawname = fileName.substr(0, fileName.find_last_of("."));

	char* buffer = nullptr; 
	uint lenght = App->fs->ReadFile(realPath.c_str(), &buffer);

	
	if (ilLoadL(IL_TYPE_UNKNOWN, (const void*)buffer, lenght))
	{
		ilSetInteger(IL_DXTC_FORMAT, IL_DXT5);
		iluFlipImage();
		size = ilSaveL(IL_DDS, NULL, 0);
		if (size > 0) {
			data = DBG_NEW ILubyte[size];
			if (ilSaveL(IL_DDS, data, size) > 0)
				App->fs->SaveUnique(output_file, data, size, LIBRARY_TEXTURES_FOLDER, rawname.c_str(), "dds");

			RELEASE_ARRAY(data);
		}
	}

	return LIBRARY_TEXTURES_FOLDER + rawname + ".dds"; 
}

bool SmileFBX::LoadModel(const char* path)
{
	rapidjson::Document doc;
	dynamic_cast<JSONParser*>(App->utilities->GetUtility("JSONParser"))->ParseJSONFile(path, doc);
	std::string name = rapidjson::GetValueByPointer(doc, "/GameObject/0/Name")->GetString();

	// TODO: load root transform (?) (identity?)
	char rawname[100];
	strcpy(rawname, std::filesystem::path(path).stem().string().c_str());
	ComponentTransform* transf = DBG_NEW ComponentTransform(math::float4x4::identity);
	GameObject* parentObj = App->object_manager->CreateGameObject(transf, rawname, App->scene_intro->rootObj); 

	rapidjson::Value& a = doc["Meshes"];
 
	// Mesh, material, transform
	for (rapidjson::SizeType i = 0; i < a.Size(); i++)
	{

		std::string path = a[i]["path"].GetString(); 
		std::string materialPath = a[i]["materialPath"].GetString();
		std::string childName = name + std::string(" (") + std::to_string(i + 1) + std::string(")"); 

		// Transform
		auto childTransfObj = a[i]["Transform"].GetObjectA(); 
	
		auto pos = childTransfObj["Position"].GetArray();
		auto rot = childTransfObj["Rotation"].GetArray();
		auto scale = childTransfObj["Scale"].GetArray();

		float3 realPos = float3(0, 0, 0), realScale = float3(0, 0, 0);
		math::Quat realRot = Quat(); float captureRot[4];

		for (rapidjson::SizeType i = 0; i < pos.Size(); i++)
			realPos[i] = pos[i].GetDouble();

		for (rapidjson::SizeType i = 0; i < scale.Size(); i++)
			realScale[i] = scale[i].GetDouble();

		for (rapidjson::SizeType i = 0; i < rot.Size(); i++)
			captureRot[i] = rot[i].GetDouble();
		realRot = Quat(captureRot[0], captureRot[1], captureRot[2], captureRot[3]); 


		ComponentTransform* transf = DBG_NEW ComponentTransform();
		GameObject* child = App->object_manager->CreateGameObject(transf, childName, parentObj);
		transf->SetLocalMatrix(math::float4x4::FromTRS(realPos, realRot, realScale));

		child->AddComponent(LoadMesh(path.c_str()));
		if (materialPath != "Empty")
			AssignTextureToObj(materialPath.c_str(), child); 


	}


	// Add to octree!!! (adds recursive!!)
	parentObj->Start();
	App->spatial_tree->OnStaticChange(parentObj, true);

	return true;
}

void SmileFBX::SaveModel(GameObject* obj, const char* path)
{
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
	writer.Key("Name");
	writer.String(obj->GetName().c_str());

	// Transform
	// Transform !! 
	writer.Key("Transform");
	writer.StartObject();

	// Pos
	writer.Key("Position");
	writer.StartArray();

	writer.Double(position.x);
	writer.Double(position.y);
	writer.Double(position.z);

	writer.EndArray();

	// Rot
	writer.Key("Rotation");
	writer.StartArray();

	writer.Double(rotation.x);
	writer.Double(rotation.y);
	writer.Double(rotation.z);
	writer.Double(rotation.w);

	writer.EndArray();

	// Scale
	writer.Key("Scale");
	writer.StartArray();

	writer.Double(scale.x);
	writer.Double(scale.y);
	writer.Double(scale.z);

	writer.EndArray();

	writer.EndObject();

	// end transform


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
			writer.String(SaveMesh(mesh->GetResourceMesh(), child, meshCount).c_str());

			writer.Key("materialPath");

			if (material)
				writer.String(SaveMaterial(material->GetTextureData()->path.c_str()).c_str());
			else
				writer.String("empty");

			// Transform !! 
			auto childPos = child->GetTransform()->GetPosition(); 
			writer.Key("Transform");
			writer.StartObject();

			// Pos
			writer.Key("Position");
			writer.StartArray();

			writer.Double(childPos.x);
			writer.Double(childPos.y);
			writer.Double(childPos.z);

			writer.EndArray();

			// Rot
			auto childRot = child->GetTransform()->GetRotation();
			writer.Key("Rotation");
			writer.StartArray();

			writer.Double(childRot.x);
			writer.Double(childRot.y);
			writer.Double(childRot.z);
			writer.Double(childRot.w);

			writer.EndArray();

			// Scale
			auto childScale = child->GetTransform()->GetScale();
			writer.Key("Scale");
			writer.StartArray();

			writer.Double(childScale.x);
			writer.Double(childScale.y);
			writer.Double(childScale.z);

			writer.EndArray();

			writer.EndObject();

			// end transform


			writer.EndObject();
			// end mesh node


		}
	}



	writer.EndArray();

	writer.EndObject();

	const char* output = buffer.GetString();
	std::string dirPath;
	App->fs->SaveUnique(dirPath, output, buffer.GetSize(), LIBRARY_MODELS_FOLDER, obj->GetName().c_str(), "json");
}



