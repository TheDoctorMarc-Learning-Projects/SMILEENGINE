#include "SmileResourceManager.h"
#include "ResourceMesh.h"
#include "ResourceTexture.h"
#include "Resource.h"
#include "SmileUtilitiesModule.h"
#include "Utility.h"
#include "RNG.h"
#include "SmileApp.h"
#include "SmileGameObjectManager.h"
#include "parshapes/par_shapes.h"

bool SmileResourceManager::Start()
{
	par_shapes_mesh* parshapes_cube = par_shapes_create_cube(); 
	Cube = DBG_NEW ResourceMesh(dynamic_cast<RNG*>(App->utilities->GetUtility("RNG"))->GetRandomUUID(), parshapes_cube, "Default");
	resources.insert(std::pair<SmileUUID, Resource*>(Cube->GetUID(), (Resource*)Cube));
	Cube->SetPreset(true); 

	par_shapes_mesh* parshapes_sphere = par_shapes_create_subdivided_sphere(2); 
	Sphere = DBG_NEW ResourceMesh(dynamic_cast<RNG*>(App->utilities->GetUtility("RNG"))->GetRandomUUID(), parshapes_sphere, "Default");
	resources.insert(std::pair<SmileUUID, Resource*>(Sphere->GetUID(), (Resource*)Sphere));
	Sphere->SetPreset(true);

	checkersTexture = DBG_NEW ResourceTexture(dynamic_cast<RNG*>(App->utilities->GetUtility("RNG"))->GetRandomUUID(), RESOURCE_TEXTURE, "Checkers texture");
	resources.insert(std::pair<SmileUUID, Resource*>(checkersTexture->GetUID(), (Resource*)checkersTexture));
	checkersTexture->SetPreset(true);
	checkersTexture->LoadCheckersOnMemory();

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
	checkersTexture = nullptr;

	return true;
}


Resource* SmileResourceManager::CreateNewResource(Resource_Type type, std::string realPath)
{
	Resource* ret = GetResourceByPath(realPath.c_str());
	if (ret)
		return ret;


	SmileUUID id = dynamic_cast<RNG*>(App->utilities->GetUtility("RNG"))->GetRandomUUID();
	switch (type) {
	case Resource_Type::RESOURCE_MESH: ret = (Resource*)DBG_NEW ResourceMesh(id, RESOURCE_MESH, realPath);   break;
	case Resource_Type::RESOURCE_TEXTURE: ret = (Resource*)DBG_NEW ResourceTexture(id, RESOURCE_TEXTURE, realPath);   break;
	}

	if (ret) resources.insert(std::pair<SmileUUID, Resource*>(ret->GetUID(), (Resource*)ret));

	return ret;
}



Resource* SmileResourceManager::GetResourceByPath(const char* Path)
{
	for (auto& resource : resources)
	{
		if (resource.second->filePath == Path)
			return resource.second;
	}


	return nullptr;
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

	if (target->referenceCount == 0 && target->IsPreset() == false)
	{
		target->FreeMemory(); 
		
		// delete from resources
		resources.erase(target->GetUID()); 

		RELEASE(target);

	}
 
}