#pragma once

#include "GameObject.h"
#include <variant>

// ----------------------------------------------------------------- [Component]
class ComponentMesh; 
class Component  
{
public:

	Component()  {}; 
	Component(GameObject* parent) : parent(parent) {};
	~Component() {}

public:
	virtual void Enable() { active = true; };
	virtual void Update() {};
	virtual void Disable() { active = false; };
	virtual void CleanUp() {};

public:
	COMPONENT_TYPE type = COMPONENT_TYPE::NO_TYPE;
	bool active = false;
	std::variant<GameObject*, ComponentMesh*> parent;
	bool unique = true;

public:
	const COMPONENT_TYPE GetComponentType() const { return type; };

	friend class GameObject;
};

