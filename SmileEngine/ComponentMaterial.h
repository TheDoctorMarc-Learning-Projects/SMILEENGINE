#pragma once

#include "Component.h"
#include "ComponentTypes.h"

// ----------------------------------------------------------------- [Transform]
class ComponentMaterial : public Component
{

public:
	ComponentMaterial();
	~ComponentMaterial();
 
	void CleanUpTextureData(); 
	void CleanUp(); 
private: 
	textureData* textureInfo = nullptr;

	friend class GameObject;
	friend class SmileFBX; 
	friend class ComponentMesh; 
};


