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
	own_mesh->points = { -own_mesh->size / 2, own_mesh->size/2, -own_mesh->size / 2, -own_mesh->size / 2, own_mesh->size / 2, -own_mesh->size / 2, own_mesh->size / 2, own_mesh->size / 2 };
	own_mesh->points3D = { -own_mesh->size / 2, own_mesh->size / 2, 0, -own_mesh->size / 2, -own_mesh->size / 2, 0, own_mesh->size / 2, -own_mesh->size / 2, 0, own_mesh->size / 2, own_mesh->size / 2, 0 };
    own_mesh->uvCoords = { 0,1,0,0,1,0,1,1 };

	// Any buffer? Color? Normals? 
}