#pragma once

#include "ResourceMesh.h"

#include "MathGeoLib/include/Math/float4x4.h"

#include "ComponentParticleEmitter.h" // had to this for an enum

class ResourceTexture;
class ResourceMeshPlane : public ResourceMesh
{
public:
	ResourceMeshPlane::ResourceMeshPlane(SmileUUID uuid, ownMeshType type, std::string path);
	ResourceMeshPlane::~ResourceMeshPlane() {}; 
	void LoadOnMemory(const char* path = { 0 }) {}; // may have a color buffer 
	void FreeMemory() {}; // may have a color buffer 

	void GenerateOwnMeshData(); 
	void BlitMeshHere(float4x4& global_transform, ResourceTexture* tex = nullptr, blendMode blendMode = blendMode::ALPHA_BLEND, float transparency = 0.f);
}; 
