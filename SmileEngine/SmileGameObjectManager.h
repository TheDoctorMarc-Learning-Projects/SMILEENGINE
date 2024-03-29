#pragma once

#include "SmileModule.h"
#include "ComponentMesh.h"
#include "parshapes/par_shapes.h"
#include <map>
#include <string>
#include "glmath.h"
#include "ComponentCamera.h"
#include "Resource.h"

par_shapes_mesh* CreateSphere();
par_shapes_mesh* CreateCube();
// TODO


class SmileGameObjectManager : public SmileModule
{
public:
	SmileGameObjectManager(SmileApp* app, bool start_enabled = true);
	~SmileGameObjectManager();

	bool Start(); 
	update_status Update(float dt);
	bool CleanUp(); 


	// Par shapes mesh tools
	par_shapes_mesh* GeneratePrimitive(std::string type);

	// GameObject tools
	GameObject* CreateGameObject(std::string name = "no name", GameObject* parent = nullptr);
	GameObject* CreateGameObject(Component* comp, std::string name = "no name", GameObject* parent = nullptr);
	GameObject* CreateGameObject(std::vector<Component*> components, std::string name = "no name", GameObject* parent = nullptr);
	void DestroyObject(GameObject* obj); 
	// Primitve tools
	void GetAllPrimitiveTypesChar(char(&)[128], bool helpInfo = false);


private: 
	void FillMaps(); 
	

public: 
	bool debug = false; // todo: change this with the ui to draw normals, etc 
	std::vector<GameObject*> toDestroy;

private: 
	std::map<std::string, par_shapes_mesh*(*)()> primitiveMap;

};