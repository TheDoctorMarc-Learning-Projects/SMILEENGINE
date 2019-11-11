#pragma once

#include "Component.h"

#include "Assimp/include/quaternion.h"
#include "Assimp/include/vector3.h"

#pragma comment (lib, "Assimp/libx86/assimp.lib")

#include "glmath.h"
#include "MathGeoLib/include/MathGeoLib.h"

// ----------------------------------------------------------------- [Transform]
class ComponentTransform : public Component
{

public:
	ComponentTransform();
	ComponentTransform(float4x4 localMat); 
	ComponentTransform(float3 position);
	~ComponentTransform();

private: 
	void CalculateGlobalMatrix(bool updateBounding = true);
	void CalculateLocalMatrix(bool updateBounding = true); 

public: 
	
	// Setters
	void SetLocalMatrix(float4x4 mat);
	void SetGlobalMatrix(float4x4 mat); 
	void ChangeRotation(Quat q); 
	void ChangePosition(float3 pos, bool recalculateMatrixes = true, bool updateBounding = true);
	void SetGlobalPosition(float3 pos);
	void AccumulatePosition(vec3 delta);
	void ChangeScale(float3 scale); 

	// Getters
	float4x4 GetLocalMatrix() const { return localMatrix; };
	float4x4 GetGlobalMatrix() const { return globalMatrix; };
	Quat GetRotation() const { return rotation; };
	float3 GetPosition() const { return position; };
	float3 GetGlobalPosition() const { return globalMatrix.TranslatePart(); };
	vec3 GetPositionVec3() const { return vec3(position.x, position.y, position.z); };
	vec3 GetGlobalPositionVec3() const { return vec3(globalMatrix.TranslatePart().x, globalMatrix.TranslatePart().y,
		globalMatrix.TranslatePart().z); };
	float3 GetScale() const { return scale; }; 

private:
	float3 position, scale; 
	Quat rotation; 
	float4x4 localMatrix; 
	float4x4 globalMatrix; 

	friend class GameObject;
};

