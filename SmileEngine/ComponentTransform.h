#pragma once

#include "Component.h"

#include "Assimp/include/quaternion.h"
#include "Assimp/include/vector3.h"

#pragma comment (lib, "Assimp/libx86/assimp.lib")

#include "MathGeoLib/include/MathGeoLib.h"
 
// ----------------------------------------------------------------- [Transform]
class ComponentTransform : public Component
{

public:
	ComponentTransform();
	ComponentTransform(float4x4 localMat); 
	~ComponentTransform();

private: 
	void CalculateGlobalMatrix(); 
	void CalculateLocalMatrix(); 

public: 
	float4x4 GetLocalMatrix() const { return localMatrix; }; 
	float4x4 GetGlobalMatrix() const { return globalMatrix; }; 

	// Proper Transformations
	void SetLocalMatrix(float4x4 mat);
	void SetGlobalMatrix(float4x4 mat); 
	void CalculateAllMatrixes(); 

	void ChangeRotation(Quat q); 
	void ChangePosition(float3 pos, bool recalculateMatrixes = true); 
	void ChangeScale(float3 scale); 

	Quat GetRotation() const { return rotation; };
	float3 GetPosition() const { return position; };
	float3 GetScale() const { return scale; }; 

private:
	float3 position, scale; 
	Quat rotation; 
	float4x4 localMatrix; 
	float4x4 globalMatrix; 

	friend class GameObject;
};

