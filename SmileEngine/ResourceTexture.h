#pragma once
#include "Resource.h"
#include "SmileSetup.h"
#include "ComponentMaterial.h"

class ResourceTexture : public Resource
{
public:
	ResourceTexture(SmileUUID uuid, Resource_Type type, std::string path) : Resource(uuid, Resource_Type::RESOURCE_TEXTURE, path) {};
	virtual ~ResourceTexture() {};
	void FreeMemory();
	void LoadOnMemory(const char* path = { 0 });

	textureData* GetTextureData() const { return textureInfo; };
private:
	textureData* textureInfo = nullptr;

	friend class SmileResourceManager; 
	friend class SmileFBX; 
}; 
