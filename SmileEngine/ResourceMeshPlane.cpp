#include "ResourceMeshPlane.h"

ResourceMeshPlane::ResourceMeshPlane(SmileUUID uuid, ownMeshType type, std::string path) : ResourceMesh(uuid, type, path)
{
	GenerateOwnMeshData();
}

void ResourceMeshPlane::GenerateOwnMeshData()
{
	own_mesh = DBG_NEW ownMeshData;
	own_mesh->type = ownMeshType::plane;
	own_mesh->size = 1.f;
	own_mesh->points = { 0, own_mesh->size, 0, 0, own_mesh->size, 0, own_mesh->size, own_mesh->size };
	own_mesh->uvCoords = { 0,1,0,0,1,0,1,1 };

	// Any buffer? Color? Normals? 
}