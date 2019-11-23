#pragma once

#include "Component.h"
#include "ComponentTypes.h"

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
private:  
	SmileUUID myresourceID;

	friend class GameObject;
	friend class SmileFBX; 
	friend class ComponentMesh; 
};


