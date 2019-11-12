#pragma once
#include <map>
#include <vector>
#include "SmileSetup.h"
#include <array>
#include <string>
 
#include "ComponentTypes.h"

class Component;
class ComponentMesh; 
class ComponentMaterial; 
class ComponentTransform;
// ----------------------------------------------------------------- [GameObject]
class GameObject
{
public: 
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
	std::array<Component*, COMPONENT_TYPE::MAX_COMPONENT_TYPES> GetComponents() const { return components; }; 
	ComponentTransform* GetTransform() const; 
	ComponentMesh* GetMesh() const; 
	ComponentMaterial* GetMaterial() const; 
	uint GetID() const { return randomID; };

	// Assign & Get data
	void SetParent(GameObject* parent); 
	void SetName(std::string name);
	GameObject* GetParent() const { return parent;  };
	std::string GetName() const { return name; }; 
	std::vector<GameObject*> GetChildrenRecursive() const; 
	std::vector<GameObject*> GetImmidiateChildren() const; 
	double GetBoundingSphereRadius() const; 

	// Main functions 
	void Start(); 
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
	uint randomID;
	std::array<Component*, COMPONENT_TYPE::MAX_COMPONENT_TYPES> components; // each component type has either one element or a vector 
	bool active = true; 
	std::string name; 
	GameObject* parent = nullptr; 

	friend class SmileGameObjectManager; 
};