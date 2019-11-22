#pragma once

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"
#include "SmileModule.h"
#include "parshapes/par_shapes.h"
#pragma comment (lib, "Assimp/libx86/assimp.lib")
#include "ResourceMesh.h"
#include <list>
#include "DevIL/include/IL/il.h"
#include "glmath.h"

#include "ComponentMesh.h"

struct globalTextureData
{
	uint totalActiveTextures = 0;
	uint rgb = 0; 
	uint rgba = 0; 
};

class ResourceMesh;

class SmileFBX : public SmileModule
{
public:
	SmileFBX(SmileApp* app, bool start_enabled = true);
	~SmileFBX();
	bool Start();
	bool CleanUp();

	// Set & Get 
	void AssignTextureToObj(const char* path, GameObject* obj); 
	void AssignCheckersTextureToObj(GameObject* mesh); // TODO: generic
	globalTextureData GetGlobalTextureinfo() const { return textInfo; };
	//void FreeMeshTexture(ComponentMesh* mesh); 
	
	// FBX
	void Load(const char* path, std::string extension);
	void LoadFBX(const char* path);
	void LoadFBXnode(aiNode* node, const aiScene* scene);
 
 
private:
	// FBX
	ModelMeshData* FillMeshBuffers(aiMesh*, ModelMeshData*);
	std::vector<std::string> ReadFBXMaterials(const aiScene*, const char* path);
	bool DoesFBXExistInAssets(const char* path); 
	bool DoesFBXHaveLinkedModel(const char* path); 
	const char* PushFBXToAssets(const char* path); 
	void GenerateModelFromFBX(const char*,const aiScene*, char* rawname);

	
	// Own File Format 


	ComponentMesh* LoadMesh(const char* path);
	std::string SaveMesh(ResourceMesh* resource, GameObject* obj, uint index = INT_MAX);
	std::string SaveMaterial(const char* path);
	bool LoadModel(const char* path);
	void SaveModel(GameObject*, const char* path);



private: 
	globalTextureData textInfo;
	GameObject* fbxParent; 
	std::string lastFbxFolder; 

public: 
	bool debug = false;
	std::string fbx_target;
	std::string models_target;
	

	friend class SmileSerialization;
};

