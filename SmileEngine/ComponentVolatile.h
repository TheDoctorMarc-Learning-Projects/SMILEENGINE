#pragma once

#include "Component.h"
#include "SmileSetup.h"

#include "MathGeoLib/include/Math/float3.h"

class ComponentVolatile : public Component
{
public: 
	ComponentVolatile() {}; 
	ComponentVolatile(float sTime); 
	ComponentVolatile(float sTime, void(*destroyFunc)());
	ComponentVolatile(float sTime, void(*destroyFunc)(), float3 speed);

	~ComponentVolatile() {}; 

	void Update(float dt);
private: 
	float currentTime = 0.f, sTime = 0.f;
	void(*destroyFunc)() = nullptr;
	float3 speed = float3::inf; 
};