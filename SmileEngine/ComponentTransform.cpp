#include "ComponentTransform.h"
//#include "Assimp/include/vector3.inl"

ComponentTransform::ComponentTransform()
{
	type = COMPONENT_TYPE::TRANSFORM; 

	matrix.translate(0, 0, 0); 
}


ComponentTransform::ComponentTransform(GameObject* parent, mat4x4 matrix) : Component(parent), matrix(matrix)
{
	type = COMPONENT_TYPE::TRANSFORM;
}


ComponentTransform::~ComponentTransform()
{

}
