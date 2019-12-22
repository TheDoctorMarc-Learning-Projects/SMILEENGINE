#pragma once

#include "MathGeoLib/include/Math/float4x4.h"

class FreeTransform  
{
public:
	FreeTransform(float4x4 parentMatrix) : parentMatrix(parentMatrix) {};
	FreeTransform() {}; 
	~FreeTransform() {}; 
private: 

	float4x4 globalMatrix;
	float4x4 localMatrix;
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
  
};
