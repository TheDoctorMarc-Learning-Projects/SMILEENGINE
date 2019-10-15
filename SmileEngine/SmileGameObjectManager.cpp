#include "SmileGameObjectManager.h"
#include "SmileSetup.h"
#include "SmileApp.h"


SmileGameObjectManager::SmileGameObjectManager(SmileApp* app, bool start_enabled) : SmileModule(app, start_enabled)
{
	FillMaps(); 
}
SmileGameObjectManager::~SmileGameObjectManager()
{
	primitiveMap.clear(); 
}

void SmileGameObjectManager::FillMaps()
{
	primitiveMap =
	{
		{"Sphere", &CreateSphere},
		{"Cube", &CreateCube},
	}; 
}

// -----------------------------------------------------------------
bool SmileGameObjectManager::Start()
{
	return true;
}
// -----------------------------------------------------------------
update_status SmileGameObjectManager::Update(float dt)
{
	return UPDATE_CONTINUE;
}

// -----------------------------------------------------------------
bool SmileGameObjectManager::CleanUp()
{
	const_cast<std::map<COMPONENT_TYPE, std::tuple<bool, bool, bool>>&>(uniquenessMap).clear();
	return true;
}

// ----------------------------------------------------------------- [Call the associated function given the wanted Primitive name]
par_shapes_mesh* SmileGameObjectManager::GeneratePrimitive(std::string type)
{
	auto item = primitiveMap.find(type);
	if (item != primitiveMap.end())
		return (*item).second(); 
		
	return nullptr; 
}

// -----------------------------------------------------------------
GameObject* SmileGameObjectManager::CreateGameObject()
{
	GameObject* ret = DBG_NEW GameObject();

	if(ret)
		App->scene_intro->objects.push_back(ret);

	return ret; 
}

// -----------------------------------------------------------------
GameObject* SmileGameObjectManager::CreateGameObject(Component* comp)
{
	GameObject* ret = DBG_NEW GameObject(comp);

	if (ret)
		App->scene_intro->objects.push_back(ret);

	return ret;
}


// -----------------------------------------------------------------
GameObject* SmileGameObjectManager::CreateGameObject(std::vector<Component*> comps)
{
	GameObject* ret = DBG_NEW GameObject(comps);

	if (ret)
		App->scene_intro->objects.push_back(ret);

	return ret;
}

// ----------------------------------------------------------------- [Primitive creation functions]
par_shapes_mesh* CreateSphere()
{
	return par_shapes_create_subdivided_sphere(2);
}


par_shapes_mesh* CreateCube()
{
	return par_shapes_create_cube();
}


