#pragma once

#include "Component.h"

#include "Assimp/include/quaternion.h"
#include "Assimp/include/vector3.h"

#pragma comment (lib, "Assimp/libx86/assimp.lib")
 
// ----------------------------------------------------------------- [Transform]
class ComponentTransform : public Component
{
public:
	struct Data
	{
		aiVector3D position, scale;
		aiQuaternion rotation;
	};

public:
	ComponentTransform();
	ComponentTransform(GameObject* parent, Data data); 
	~ComponentTransform(); 

private:
	Data data;

	friend class GameObject;
};

