#pragma once
#include "Resource.h"
#include "SmileSetup.h"
#include "ComponentMaterial.h"

class ResourceTexture : public Resource
{
public:
	ResourceTexture(SmileUUID uuid) : Resource(uuid, Resource_Type::RESOURCE_TEXTURE) {};
	virtual ~ResourceTexture() {};
	void FreeMemory();

	textureData* GetTextureData() const { return textureInfo; };
private:
	textureData* textureInfo = nullptr;

};
