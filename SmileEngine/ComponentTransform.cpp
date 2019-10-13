#include "ComponentTransform.h"
//#include "Assimp/include/vector3.inl"

ComponentTransform::ComponentTransform()
{
	type = COMPONENT_TYPE::TRANSFORM; 

	/*data.position.Set(0, 0, 0);
	data.scale.Set(1, 1, 1);
	data.rotation.w = 0; data.rotation.x = 0; data.rotation.y = 0; data.rotation.z = 0;*/
}


ComponentTransform::ComponentTransform(GameObject* parent, Data data) : Component(parent), data(data)
{
	type = COMPONENT_TYPE::TRANSFORM;
}


ComponentTransform::~ComponentTransform()
{

}
