#pragma once

#include "Component.h"

#include "Assimp/include/quaternion.h"
#include "Assimp/include/vector3.h"

#pragma comment (lib, "Assimp/libx86/assimp.lib")

#include "glmath.h"
 
// ----------------------------------------------------------------- [Transform]
class ComponentTransform : public Component
{

public:
	ComponentTransform();
	ComponentTransform(GameObject* parent, mat4x4 matrix);
	~ComponentTransform(); 

public:
	mat4x4 matrix; 

	friend class GameObject;
};

