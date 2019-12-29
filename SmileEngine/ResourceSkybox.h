#pragma once

#include "Resource.h"
#include <map>
#include "MathGeoLib/include/Math/float4x4.h"

class ResourceTexture; 
class ResourceMeshPlane; 
class ResourceSkybox : public Resource
{
public: 
	ResourceSkybox(SmileUUID uuid, Resource_Type type, std::string texPaths[6], float radius);
	~ResourceSkybox() {};

	void Draw(); 
	float4x4 GetPlaneTransform(uint index); 

private: 
	float radius = 0.f; 
	ResourceMeshPlane* planes[6]; 
	ResourceTexture* textures[6];
};
