#pragma once

#include "Component.h"

enum Mesh_Type
{
	Primitive,
	Model
};

// ----------------------------------------------------------------- [Mesh]
class ComponentMesh : public Component
{
public:
	ComponentMesh() { type = COMPONENT_TYPE::MESH; }; 
	~ComponentMesh() {};

	// TODO: same logic with components as GameObject (Enable, Disable, Update, CleanUp)
	// TODO: the mesh can either be "OpenGL importer related" or a par_shapes_mesh
	// TODO: use the geometry generator to create a GameObject that cointains a ComponentMesh with the par_shapes
	// TODO: use another constructor to recieve a mesh from the old Mesh structure + bring it to this header 

private:
	std::variant<Component*, std::vector<Component*>> components[COMPONENT_TYPE::MAX_COMPONENT_TYPES]; // each component type has either one element or a vector 

	friend class GameObject;
};