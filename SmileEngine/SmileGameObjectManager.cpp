#include "SmileGameObjectManager.h"
#include "SmileSetup.h"
#include "SmileApp.h"
#include "Glew/include/GL/glew.h" 
#include "SmileUtilitiesModule.h"
#include "Utility.h"
#include "RNG.h"
#include "ResourceMesh.h"
SmileGameObjectManager::SmileGameObjectManager(SmileApp* app, bool start_enabled) : SmileModule(app, start_enabled)
{
	FillMaps(); 
}
SmileGameObjectManager::~SmileGameObjectManager()
{
	primitiveMap.clear(); 
	componentTypeMap.clear(); 
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
	if (toDestroy.size() > 0)
	{
		for (auto it = toDestroy.begin(); it != toDestroy.end(); ++it)
			DestroyObject((*it)); 

		toDestroy.clear(); 
	}

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
	GameObject * ret;  
	ret = DBG_NEW GameObject(name, parent);
	ret->randomID = dynamic_cast<RNG*>(App->utilities->GetUtility("RNG"))->GetRandomUUID(); 
	return ret;
}

// -----------------------------------------------------------------
GameObject* SmileGameObjectManager::CreateGameObject(Component* comp, std::string name, GameObject* parent)
{
	GameObject* ret;
	ret = DBG_NEW GameObject(comp, name, parent);
	ret->randomID = dynamic_cast<RNG*>(App->utilities->GetUtility("RNG"))->GetRandomUUID();
	return ret; 
}


// -----------------------------------------------------------------
GameObject* SmileGameObjectManager::CreateGameObject(std::vector<Component*> components, std::string name, GameObject* parent)
{
	GameObject* ret;
	ret = DBG_NEW GameObject(components, name, parent);
	ret->randomID = dynamic_cast<RNG*>(App->utilities->GetUtility("RNG"))->GetRandomUUID();
	return ret;
}


// -----------------------------------------------------------------
void SmileGameObjectManager::DestroyObject(GameObject* obj)
{
	if (obj)
	{
	 
		// 1) Clean myself, remove from octree, clean children (remove from octree), release children
		obj->CleanUp(); 
		
		// 2) erase from parent's list 
		GameObject* parent = obj->GetParent(); 
		if (parent)
		{

			for (auto item = parent->childObjects.begin(); item != parent->childObjects.end(); ++item)
			{
				if ((*item) != nullptr && (*item) == obj)
				{

					item = parent->childObjects.erase(item);
					break;

				}

			}
		}

		// 3) finally destroy the object
		RELEASE(obj); 
	}
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
