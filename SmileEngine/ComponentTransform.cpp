#include "ComponentTransform.h"
#include "SmileApp.h"
#include "SmileScene.h"

ComponentTransform::ComponentTransform()
{
	type = COMPONENT_TYPE::TRANSFORM; 
	CalculateGlobalMatrix(); 
}

ComponentTransform::~ComponentTransform()
{

}

void ComponentTransform::CalculateAllMatrixes()
{
	CalculateLocalMatrix();
	CalculateGlobalMatrix();
}

void ComponentTransform::CalculateGlobalMatrix()
{
	if (std::get<GameObject*>(parent) != nullptr)
	{
		globalMatrix = dynamic_cast<ComponentTransform*>(std::get<Component*>((std::get<GameObject*>(parent)->GetComponent(TRANSFORM))))->GetGlobalMatrix()
			* localMatrix; 
	}
	else
		globalMatrix = localMatrix; 
}

void ComponentTransform::CalculateLocalMatrix()
{
	localMatrix = float4x4::FromTRS(position, rotation, scale); 
}

void ComponentTransform::SetGlobalMatrix(float4x4 mat)
{
	globalMatrix = mat;
	localMatrix.Decompose(position, rotation, scale);

	CalculateAllMatrixes();
}


void ComponentTransform::SetLocalMatrix(float4x4 mat)
{
	localMatrix = mat; 
	localMatrix.Decompose(position, rotation, scale);

	CalculateGlobalMatrix(); 
}

void ComponentTransform::ChangePosition(float3 pos)
{
	position = pos; 
	CalculateAllMatrixes(); 
}

void ComponentTransform::ChangeScale(float3 sc)
{
	scale = sc; 
	CalculateAllMatrixes();
}

void ComponentTransform::ChangeRotation(Quat q)
{
	rotation = q; 
	CalculateAllMatrixes();
}