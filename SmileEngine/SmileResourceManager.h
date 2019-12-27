#pragma once

#include "SmileModule.h"
#include <map>

class Resource;
enum Resource_Type;
class ResourceMesh; 
class ResourceTexture; 
class ResourceMeshPlane; 
class ResourceSkybox; 
class SmileResourceManager : public SmileModule
{
public:
	SmileResourceManager(SmileApp* app, bool start_enabled = true) : SmileModule(app, start_enabled) {};
	~SmileResourceManager() {};

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	// The real stuff
	SmileUUID Find(const char* file_in_assets) const;
	const Resource* Get(SmileUUID id) const;
	Resource* GetResourceByPath(const char* Path);
	Resource* Get(SmileUUID uid);
	Resource* CreateNewResource(Resource_Type type, std::string assetPath);
	void UpdateResourceReferenceCount(SmileUUID resource, int add); // add is either 1 or -1

public: 
	Resource* CreateMaterialFromPath(const char* path); 

public: 
	ResourceMeshPlane* Plane;
	ResourceMesh* Cube;
	ResourceMesh* Sphere;
	ResourceTexture* checkersTexture;
	ResourceSkybox* skybox; 
	std::map<SmileUUID, Resource*> resources;


}; 

