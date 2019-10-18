#pragma once

#include "SmileModule.h"
#include "ComponentMesh.h"
#include "parshapes/par_shapes.h"
#include <map>
#include <string>

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

	// Component Mesh tools
	void AssignTextureImageToMesh(const char* path, ComponentMesh* mesh)
	{ 
		mesh->AssignTexture(path); 
	};

	// Par shapes mesh tools
	par_shapes_mesh* GeneratePrimitive(std::string type);

	// GameObject tools
	GameObject* CreateGameObject(); 
	GameObject* CreateGameObject(Component* comp);
	GameObject* CreateGameObject(std::vector<Component*> comps);

	// Primitve tools
	void GetAllPrimitiveTypesChar(char(&)[128], bool helpInfo = false);


private: 
	void FillMaps(); 

public: 
	bool debug = false; // todo: change this with the ui to draw normals, etc 

private: 
	std::map<std::string, par_shapes_mesh*(*)()> primitiveMap;

};