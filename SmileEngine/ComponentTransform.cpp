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


void ComponentTransform::CalculateGlobalMatrix(bool updateBounding)
{
	// Calculate the global matrix
	if (parent != nullptr && parent != App->scene_intro->rootObj)
	{
		float4x4 parentMat = dynamic_cast<ComponentTransform*>(parent->GetParent()->GetComponent(TRANSFORM))->GetGlobalMatrix(); 
		globalMatrix = parentMat * localMatrix;
	}
	else
		globalMatrix = localMatrix; 


	// Warn the parent! (To update OBB, ABB...). Caution! If it has a camera, nanai.  
	// The camera is updated in the gui, because if we update it here, a circular situation occurs
	if(parent && parent->GetCamera() == nullptr)
		parent->OnTransform(updateBounding);
}

void ComponentTransform::CalculateLocalMatrix(bool updateBounding)
{
	localMatrix = float4x4::FromTRS(position, rotation, scale); 
	CalculateGlobalMatrix(updateBounding);

	if (parent)
	{
		// Inform my GameObject's children
		for (auto& child : parent->GetChildrenRecursive())
			child->GetTransform()->CalculateGlobalMatrix(updateBounding);
	}

}

void ComponentTransform::SetGlobalMatrix(float4x4 mat)
{
	LOG("Object has a position before global update --> (%f,%f,%f)", GetGlobalPosition().x, GetGlobalPosition().y, GetGlobalPosition().z); 
	if (parent != nullptr && parent != App->scene_intro->rootObj)
	{
		float4x4 parentMat = dynamic_cast<ComponentTransform*>(parent->GetParent()->GetComponent(TRANSFORM))->GetGlobalMatrix();
		localMatrix = parentMat.Inverted() * (globalMatrix = mat);
	}
	else
		localMatrix =  (globalMatrix = mat);
	LOG("Object has a position after global update --> (%f,%f,%f)", GetGlobalPosition().x, GetGlobalPosition().y, GetGlobalPosition().z);
}

void ComponentTransform::SetLocalMatrix(float4x4 mat)
{
	localMatrix = mat; 
	localMatrix.Decompose(position, rotation, scale);

	CalculateGlobalMatrix(); 
}

void ComponentTransform::SetupTransform(float4x4 mat)
{
	localMatrix = mat;
	localMatrix.Decompose(position, rotation, scale);
}

void ComponentTransform::ChangePosition(float3 pos, bool recalculateMatrixes, bool updateBounding)
{
	position = pos; 
	if(recalculateMatrixes)
		CalculateLocalMatrix(updateBounding);
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

void ComponentTransform::UpdateTransform(float values[3][3]) // position, rotation (rads), scale
{
	if (values[0][0] != position.x || values[0][1] != position.y || values[0][2] != position.z)
		ChangePosition(math::float3(values[0]));

	math::Quat newRot = math::Quat::FromEulerXYZ(values[1][0], values[1][1], values[1][2]); 
	if (newRot.x != rotation.x || newRot.y != rotation.y || newRot.z != rotation.z || newRot.w != rotation.w)
		ChangeRotation(newRot);

	if (values[2][0] != scale.x || values[2][1] != scale.y || values[2][2] != scale.z)
		ChangeScale(math::float3(values[2]));

	// if the parent has a camera, update it from here, otherwise it is internally updated in transform
	if (parent->GetCamera() != nullptr)
		parent->OnTransform();
}