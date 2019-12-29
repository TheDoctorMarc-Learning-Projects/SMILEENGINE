#pragma once

#include "Component.h"
#include "ComponentTypes.h"

struct MaterialData
{
	float transparency = 0.f; 
};

class ResourceTexture; 
// ----------------------------------------------------------------- [Transform]
class ComponentMaterial : public Component
{

public:
	ComponentMaterial(SmileUUID uid, std::string name = "no name");
	~ComponentMaterial();
 
	void CleanUp(); 

	ResourceTexture* GetResourceTexture() const; 
	textureData* GetTextureData() const; 
	MaterialData* GetMaterialData() const {	return materialData;};

private:  
	SmileUUID myresourceID;
	MaterialData* materialData; 

	friend class GameObject;
	friend class SmileFBX; 
	friend class ComponentMesh; 
};


