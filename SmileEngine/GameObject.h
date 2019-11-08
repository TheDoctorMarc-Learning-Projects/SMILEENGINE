#pragma once
#include <map>
#include <vector>
#include "SmileSetup.h"
#include <array>
#include <string>
 
#include "ComponentTypes.h"

#define debugLineSize 1.8
#define debugLineHead 0.3

class Component;
class ComponentMesh; 
class ComponentTransform; 
// ----------------------------------------------------------------- [GameObject]
class GameObject
{
public:
	GameObject(GameObject* parent); 
	GameObject(std::string name = "no name", GameObject* parent = nullptr);  
	GameObject(Component* comp, std::string name = "no name", GameObject* parent = nullptr);
	GameObject(std::vector<Component*> components, std::string name = "no name", GameObject* parent = nullptr);
	~GameObject() {}; 

private:
	void FillComponentBuffers();

public: 

	// Components
	bool AddComponent(Component* comp); // you can add it to the GameObject or to a mesh
	void SetupTransformAtMeshCenter(); 

	Component* GetComponent(COMPONENT_TYPE type) const { return components[type]; }
	ComponentTransform* GetTransform() const; 
	std::array<Component*, COMPONENT_TYPE::MAX_COMPONENT_TYPES> GetComponents() const { return components; }; 

	// Assign & Get data
	void SetParent(GameObject* parent); 
	void SetName(std::string name);
	GameObject* GetParent() const { return parent;  };
	std::string GetName() const { return name; }; 
	std::vector<GameObject*> GetChildrenRecursive() const; 
	std::vector<GameObject*> GetImmidiateChildren() const; 
	double GetBoundingSphereRadius() const; 

	// Main functions 
	virtual void Start(); 
	virtual void Enable(); 
	virtual void Update();
	virtual void Disable();
	virtual void CleanUp(); 

	// Other
	virtual void OnTransform(bool [3]); // pos, rot, sc 

	// State
	bool IsActive() const { return active; };

	// Debug purposes
	void DrawAxis(); 

public:
	std::vector<GameObject*> childObjects;

private: 
	std::array<Component*, COMPONENT_TYPE::MAX_COMPONENT_TYPES> components; // each component type has either one element or a vector 
	bool active = true; 
	std::string name; 
	GameObject* parent = nullptr; 

	friend class SmileGameObjectManager; 
	friend class GameObjectCamera; 
};