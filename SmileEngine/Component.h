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
	virtual void Start() { active = true;  };
	virtual void Enable() { active = true; };
	virtual void Update(float dt = 0) {};
	virtual void Disable() { active = false; };
	virtual void CleanUp() {};

	virtual void OnTransform() {};
	// just after having a parent 

	std::string GetName() const { return name; };
	void SetName(std::string name) { this->name = name; };
	GameObject* GetParent() const { return parent; };
	void SetParent(GameObject* parent) { this->parent = parent; }; 

private:
	COMPONENT_TYPE type = COMPONENT_TYPE::NO_TYPE;
	bool active = false;
	GameObject* parent = nullptr;
 

public:
	const COMPONENT_TYPE GetComponentType() const { return type; };
	std::string name;

	friend class GameObject;
	friend class ComponentMaterial; 
	friend class ComponentMesh;
	friend class ComponentTransform; 
	friend class ComponentCamera; 
	friend class ComponentParticleEmitter; 
};

