#include "SmileGameObjectManager.h"
#include "SmileSetup.h"
#include "SmileApp.h"
#include "Glew/include/GL/glew.h" 

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

	std::string typesStr = ""; 
	for (auto prim : primitiveMap)
	{
		typesStr += prim.first; 
		typesStr += "/n"; 
	}


}

// -----------------------------------------------------------------
bool SmileGameObjectManager::Start()
{
	return true;
}
// -----------------------------------------------------------------
update_status SmileGameObjectManager::Update(float dt)
{
	if(App->scene_intro->rootObj)
		App->scene_intro->rootObj->Update();

	return UPDATE_CONTINUE;
}

// -----------------------------------------------------------------
bool SmileGameObjectManager::CleanUp()
{
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
GameObject* SmileGameObjectManager::CreateGameObject(std::string name, GameObject* parent)
{
	return DBG_NEW GameObject(name, parent);
}

// -----------------------------------------------------------------
GameObject* SmileGameObjectManager::CreateGameObject(Component* comp, std::string name, GameObject* parent)
{
	return DBG_NEW GameObject(comp, name, parent);
}


// -----------------------------------------------------------------
GameObject* SmileGameObjectManager::CreateGameObject(std::vector<Component*> components, std::string name, GameObject* parent)
{
	return DBG_NEW GameObject(components, name, parent);
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

// help 
void SmileGameObjectManager::GetAllPrimitiveTypesChar(char(&array)[128], bool helpInfo)
{
	if (helpInfo)
	{
		strcat(array, "Available types:");
		strcat(array, "\n");
	}

	for (auto& string : primitiveMap)
	{
		strcat(array, string.first.c_str());
		strcat(array, "\n");
	}
}
