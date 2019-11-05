#pragma once

#include "SmileModule.h"
#include "ComponentMesh.h"
#include "parshapes/par_shapes.h"
#include <map>
#include <string>
#include "glmath.h"


par_shapes_mesh* CreateSphere();
par_shapes_mesh* CreateCube();
// TODO

class GameObjectCamera; 
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

	// Others
	GameObjectCamera* CreateCamera(GameObject* parent, vec3 Position, vec3 Reference); 

	// Primitve tools
	void GetAllPrimitiveTypesChar(char(&)[128], bool helpInfo = false);


private: 
	void FillMaps(); 

public: 
	bool debug = false; // todo: change this with the ui to draw normals, etc 

private: 
	std::map<std::string, par_shapes_mesh*(*)()> primitiveMap;

};