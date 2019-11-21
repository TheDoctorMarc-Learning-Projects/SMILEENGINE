#include "SmileResourceManager.h"
#include "ResourceMesh.h"
#include "ResourceTexture.h"
#include "SmileUtilitiesModule.h"
#include "Utility.h"
#include "RNG.h"
#include "SmileApp.h"

bool SmileResourceManager::Start()
{

	return true; 
}

update_status SmileResourceManager::Update(float dt)
{
	return update_status::UPDATE_CONTINUE; 
}

bool SmileResourceManager::CleanUp()
{
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

 