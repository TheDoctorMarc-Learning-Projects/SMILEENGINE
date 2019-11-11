#include "ComponentTransform.h"
#include "SmileApp.h"
#include "SmileScene.h"


ComponentTransform::ComponentTransform()
{
	SetName("Transform"); 
	type = COMPONENT_TYPE::TRANSFORM; 
	SetLocalMatrix(float4x4::identity);
}

ComponentTransform::ComponentTransform(float4x4 localMat)
{
	SetName("Transform");
	SetLocalMatrix(localMat);
	type = COMPONENT_TYPE::TRANSFORM;
}

ComponentTransform::ComponentTransform(float3 position)
{ 
	// Take the position and create a matrix
	float4x4 mat = float4x4(); 
	mat.SetIdentity(); 
	mat.SetTranslatePart(position); 

	//Proper Setup
	SetName("Transform");
	SetLocalMatrix(mat);
	type = COMPONENT_TYPE::TRANSFORM;
}

ComponentTransform::~ComponentTransform()
{

}


void ComponentTransform::CalculateGlobalMatrix()
{
	if (parent != nullptr && parent != App->scene_intro->rootObj)
	{
		float4x4 parentMat = dynamic_cast<ComponentTransform*>(parent->GetParent()->GetComponent(TRANSFORM))->GetGlobalMatrix(); 
		globalMatrix = parentMat * localMatrix;
	}
	else
		globalMatrix = localMatrix; 

	if (!parent || parent->GetName() == "Debug Camera")
		return; 

	LOG("GameObject: '%s' is in the global position (%f, %f, %f) and in the local position (%f, %f, %f)",
		GetParent()->GetName().c_str(), GetGlobalPosition().x, GetGlobalPosition().y, GetGlobalPosition().z,
		position.x, position.y, position.z); 
}

void ComponentTransform::CalculateLocalMatrix()
{
	localMatrix = float4x4::FromTRS(position, rotation, scale); 
	CalculateGlobalMatrix(); 

	// Update children
	if (!parent)
		return; 
	for (auto& child : parent->GetChildrenRecursive())
		child->GetTransform()->CalculateGlobalMatrix(); 

}

void ComponentTransform::SetGlobalMatrix(float4x4 mat)
{
	globalMatrix = mat;
	CalculateLocalMatrix();
}

void ComponentTransform::SetLocalMatrix(float4x4 mat)
{
	localMatrix = mat; 
	localMatrix.Decompose(position, rotation, scale);

	CalculateGlobalMatrix(); 
}

void ComponentTransform::ChangePosition(float3 pos, bool recalculateMatrixes)
{
	position = pos; 
	if(recalculateMatrixes)
		CalculateLocalMatrix();
}

void ComponentTransform::SetGlobalPosition(float3 pos)
{
	globalMatrix.SetTranslatePart(pos); 
}

void ComponentTransform::AccumulatePosition(vec3 delta)
{
	position = position + float3(delta.x, delta.y, delta.z); 
	CalculateLocalMatrix();
}

void ComponentTransform::ChangeScale(float3 sc)
{
	scale = sc; 
	CalculateLocalMatrix();
}

void ComponentTransform::ChangeRotation(Quat q)
{
	if (abs(q.x) > 1 || abs(q.y) > 1 || abs(q.z) > 1 || abs(q.w) > 1)
		return; 
		
	rotation = q; 
	CalculateLocalMatrix();
}