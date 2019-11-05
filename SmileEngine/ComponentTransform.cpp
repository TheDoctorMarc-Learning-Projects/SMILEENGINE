#include "ComponentTransform.h"
#include "SmileApp.h"
#include "SmileScene.h"


ComponentTransform::ComponentTransform()
{
	SetName("Transform"); 
	SetLocalMatrix(float4x4::identity); 
	type = COMPONENT_TYPE::TRANSFORM; 
	CalculateGlobalMatrix(); 
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
	if (parent != nullptr)
	{
		globalMatrix = dynamic_cast<ComponentTransform*>(parent->GetParent()->GetComponent(TRANSFORM))->GetGlobalMatrix()
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

void ComponentTransform::ChangePosition(float3 pos, bool recalculateMatrixes)
{
	position = pos; 
	if(recalculateMatrixes)
		CalculateAllMatrixes();
}

void ComponentTransform::AccumulatePosition(vec3 delta)
{
	position = position + float3(delta.x, delta.y, delta.z); 
	CalculateAllMatrixes();
}

void ComponentTransform::ChangeScale(float3 sc)
{
	scale = sc; 
	CalculateAllMatrixes();

	// Wohoa! Radius and center will be fucked up! 
	parent->SetupTransformAtMeshCenter(); 
}

void ComponentTransform::ChangeRotation(Quat q)
{
	if (abs(q.x) > 1 || abs(q.y) > 1 || abs(q.z) > 1 || abs(q.w) > 1)
	{
		MessageBox(nullptr, TEXT("Please enter a valid quaternion"), TEXT("You violed rotation requirements"), MB_OK);
		return; 
	}
		
	rotation = q; 
	CalculateAllMatrixes();
}