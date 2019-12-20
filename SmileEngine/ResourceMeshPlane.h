#pragma once

#include "ResourceMesh.h"

class ResourceMeshPlane : public ResourceMesh
{
public:
	ResourceMeshPlane::ResourceMeshPlane(SmileUUID uuid, ownMeshType type, std::string path);
	ResourceMeshPlane::~ResourceMeshPlane() {}; 
	void LoadOnMemory(const char* path = { 0 }) {}; // may have a color buffer 
	void FreeMemory() {}; // may have a color buffer 

	void GenerateOwnMeshData(); 
}; 
