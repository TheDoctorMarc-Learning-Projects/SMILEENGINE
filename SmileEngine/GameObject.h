#pragma once
#include <map>
#include <vector>
#include "SmileSetup.h"
#include <variant>
#include <string>
 
#include "ComponentTypes.h"

class Component;
class ComponentMesh; 
// ----------------------------------------------------------------- [GameObject]
class GameObject
{
public: 
	GameObject(GameObject* parent = nullptr);  
	GameObject(Component* comp, GameObject* parent = nullptr);
	GameObject(std::vector<Component*> components, GameObject* parent = nullptr);
	~GameObject() {}; 

private:
	void FillComponentBuffers();

public: 

	// Components
	bool AddComponent(Component* comp); // you can add it to the GameObject or to a mesh
	bool AddComponentToMesh(Component* comp, ComponentMesh* mesh); // you can add it to the GameObject or to a mesh

	std::variant<Component*, std::vector<Component*>> GetComponent(COMPONENT_TYPE type) const
	{
		return components[type]; 
	}

	// Assign & Get data
	void SetParent(GameObject* parent); 
	void SetName(std::string name) { this->name = name; };
	GameObject* GetParent() const { return parent;  };
	std::string GetName() const { return name; }; 
	std::vector<GameObject*> GetChildrenRecursive(); 

	// Main functions 
	void Enable(); 
	void Update();
	void Disable();
	void CleanUp(); 

	// State
	bool IsActive() const { return active; };
	void OnTransform(); 

public:
	std::vector<GameObject*> childObjects;

private: 
	std::variant<Component*, std::vector<Component*>> components[COMPONENT_TYPE::MAX_COMPONENT_TYPES]; // each component type has either one element or a vector 
	bool active = true; 
	std::string name; 
	GameObject* parent; 

	friend class SmileGameObjectManager; 
};