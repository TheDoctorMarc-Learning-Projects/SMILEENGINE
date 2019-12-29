#pragma once

#include "MathGeoLib/include/Math/float4x4.h"

struct FreeTransform  
{
private: 

	float4x4 globalMatrix = float4x4::identity;
	float4x4 localMatrix = float4x4::identity;
	float4x4 parentMatrix = float4x4::identity; 

public: 
	float4x4 GetGlobalMatrix() const { return globalMatrix; };

public: 

	inline void UpdateLocalMatrix(float4x4& matrix)
	{
		globalMatrix = parentMatrix * (localMatrix = matrix);
	}

	inline void UpdateGlobalMatrix(float4x4& matrix)
	{
		localMatrix = parentMatrix.Inverted() * (globalMatrix = matrix);
	}
  
	inline void ChangeRotation(Quat rot)
	{
		globalMatrix.SetRotatePart(rot); 
		UpdateGlobalMatrix(globalMatrix); 
	}

	inline void ChangeScale(float3 scale)
	{
		localMatrix = localMatrix.FromTRS(localMatrix.TranslatePart(), localMatrix.RotatePart(), scale);
		UpdateLocalMatrix(localMatrix);
	}


	friend class ComponentParticleEmitter; 
	friend class SmileSerialization; 
};


static FreeTransform null; 
