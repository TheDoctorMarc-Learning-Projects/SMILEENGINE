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

	virtual void OnTransform() {}; 
	std::string GetName() const { return name; };
	void SetName(std::string name) { this->name = name; };

private:
	COMPONENT_TYPE type = COMPONENT_TYPE::NO_TYPE;
	bool active = false;
	std::variant<GameObject*, ComponentMesh*> parent;
 

public:
	const COMPONENT_TYPE GetComponentType() const { return type; };
	std::string name;

	friend class GameObject;
	friend class ComponentMaterial; 
	friend class ComponentMesh;
	friend class ComponentTransform; 
};

