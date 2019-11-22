#include "SmileResourceManager.h"
#include "ResourceMesh.h"
#include "ResourceTexture.h"
#include "SmileUtilitiesModule.h"
#include "Utility.h"
#include "RNG.h"
#include "SmileApp.h"
#include "SmileGameObjectManager.h"
#include "parshapes/par_shapes.h"

bool SmileResourceManager::Start()
{
	par_shapes_mesh* parshapes_cube = par_shapes_create_cube(); 
	Cube = DBG_NEW ResourceMesh(dynamic_cast<RNG*>(App->utilities->GetUtility("RNG"))->GetRandomUUID(), parshapes_cube);
	resources.insert(std::pair<SmileUUID, Resource*>(Cube->GetUID(), (Resource*)Cube));

	par_shapes_mesh* parshapes_sphere = par_shapes_create_subdivided_sphere(2); 
	Sphere = DBG_NEW ResourceMesh(dynamic_cast<RNG*>(App->utilities->GetUtility("RNG"))->GetRandomUUID(), parshapes_sphere);
	resources.insert(std::pair<SmileUUID, Resource*>(Sphere->GetUID(), (Resource*)Sphere));

	return true; 
}

update_status SmileResourceManager::Update(float dt)
{
	return update_status::UPDATE_CONTINUE; 
}

bool SmileResourceManager::CleanUp()
{
	for (auto item = resources.begin(); item != resources.end(); ++item)
	{
		(*item).second->FreeMemory(); 
		RELEASE((*item).second); 
	}
	resources.clear(); 

	Cube = nullptr; 
	Sphere = nullptr; 

	return true;
}

Resource* SmileResourceManager::CreateNewResource(Resource_Type type)
{
	Resource* ret = nullptr;
	SmileUUID id = dynamic_cast<RNG*>(App->utilities->GetUtility("RNG"))->GetRandomUUID(); 
	switch (type) {
		case Resource_Type::RESOURCE_MESH : ret = (Resource*)DBG_NEW ResourceMesh(id);   break;
		case Resource_Type::RESOURCE_TEXTURE : ret = (Resource*)DBG_NEW ResourceTexture(id);   break;
 	}
	
	if (ret) resources[id] = ret;

	return ret; 
}

SmileUUID SmileResourceManager::ImportFile(const char* new_file_in_assets, Resource_Type type)
{
	// TODO: call whatever imported needed according to extension and create a resource (?)
	SmileUUID ret = 0;
	bool import_ok = false;
	std::string written_file;

	switch (type)
	{
	case Resource_Type::RESOURCE_MESH:
		//import_ok = App->fbx->
		break;
	case Resource_Type::RESOURCE_TEXTURE:

		break;
	default:
		break;
	}
	if (import_ok)
	{
		Resource* res = CreateNewResource(type);
		res->SetFile(new_file_in_assets);
		res->SetImportedFile(written_file);
		ret = res->GetUID();
	}

	return ret; 
}

Resource* SmileResourceManager::Get(SmileUUID uid)
{
	std::map<SmileUUID, Resource*>::iterator it = resources.find(uid);
	if (it != resources.end())
		return it->second;
	return nullptr;
}

void SmileResourceManager::UpdateResourceReferenceCount(SmileUUID resource, int add)
{
	if (add != 1 && add != -1)
		return; 

	Resource* target = Get(resource); 
	if (target == nullptr)
		return; 

	target->referenceCount += add; 

	if (target->referenceCount == 0)
	{
		target->FreeMemory(); 
		
		// delete from resources
		resources.erase(target->GetUID()); 

		RELEASE(target);

	}
 
}