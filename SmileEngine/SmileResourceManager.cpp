#include "SmileResourceManager.h"
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

Resource* SmileResourceManager::CreateNewResource(Resource::Type type)
{
	Resource* ret = nullptr;
	SmileUUID id = dynamic_cast<RNG*>(App->utilities->GetUtility("RNG"))->GetRandomUUID(); 
	switch (type) {
	//case Resource::MESH : ret = (Resource*) DBG_NEW ResourceMesh(id);   break;
 	}
	
	if (ret) resources[id] = ret;

	return ret; 
}

SmileUUID SmileResourceManager::ImportFile(const char* new_file_in_assets)
{
	// TODO: call whatever imported needed according to extension and create a resource (?)

	return 0; 
}

 