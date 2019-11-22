#pragma once

#include "SmileModule.h"
#include <map>

class Resource;
enum Resource_Type;
class ResourceMesh; 
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
	SmileUUID ImportFile(const char* new_file_in_assets, Resource_Type type);
	const Resource* Get(SmileUUID id) const;
	Resource* Get(SmileUUID uid);
	Resource* CreateNewResource(Resource_Type type);
	void UpdateResourceReferenceCount(SmileUUID resource, int add); // add is either 1 or -1

public: 
	ResourceMesh* Cube;
	ResourceMesh* Sphere;
private:
	std::map<SmileUUID, Resource*> resources;


}; 
