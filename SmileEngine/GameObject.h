#pragma once

#include <map>
#include <vector>
#include "SmileSetup.h"
#include <array>
#include <string> 
#include "ComponentTypes.h"
#include "MathGeoLib/include/Geometry/AABB.h"
#include  "MathGeoLib/include/Geometry/OBB.h"

#define debugLineSize 1.8
#define debugLineHead 0.3

class Component;
class ComponentMesh; 
class ComponentTransform; 
class ComponentCamera; 

struct BoundingData
{
	math::AABB AABB; 
	math::OBB OBB; 
};

struct DebugData
{
	bool AABB = true; 
	bool OBB = true; 
};

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

	// Setters
	bool AddComponent(Component* comp);  
	void SetParent(GameObject* parent);
	void SetName(std::string name);


	// Getters
	Component* GetComponent(COMPONENT_TYPE type) const { return components[type]; }
	ComponentTransform* GetTransform() const;
	ComponentMesh* GetMesh() const;
	ComponentCamera* GetCamera() const;
	std::array<Component*, COMPONENT_TYPE::MAX_COMPONENT_TYPES> GetComponents() const { return components; };

	BoundingData GetBoundingData() const { return boundingData; }; 
	GameObject* GetParent() const { return parent; };
	std::string GetName() const { return name; };
	std::vector<GameObject*> GetChildrenRecursive() const;
	std::vector<GameObject*> GetImmidiateChildren() const;
	float GetBoundingSphereRadius() const;

	
		// Main functions 
	virtual void Start(); 
	virtual void Enable(); 
	virtual void Update();
	virtual void Disable();
	virtual void CleanUp(); 

		// Other
	virtual void OnTransform(bool updateBounding = true);
	bool IsActive() const { return active; };
	void DrawAxis(); 

		// Synchro with meshes
	void SetupWithMesh(); // calls the two methods below: 
	void PositionTransformAtMeshCenter();
	void SetupBounding();  
	void UpdateBounding();

private: 
	void Debug(); 

public:
	std::vector<GameObject*> childObjects;
	DebugData debugData; 

private: 
	std::array<Component*, COMPONENT_TYPE::MAX_COMPONENT_TYPES> components; // each component type has either one element or a vector 
	bool active = true; 
	std::string name; 
	GameObject* parent = nullptr; 
	BoundingData boundingData; 

	friend class SmileGameObjectManager; 
	friend class ComponentCamera; 
};