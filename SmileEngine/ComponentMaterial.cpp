#include "ComponentMaterial.h"
#include "Glew/include/GL/glew.h"
#include "SmileResourceManager.h"
#include "ResourceTexture.h"
#include "SmileApp.h"


ComponentMaterial::ComponentMaterial(SmileUUID uid, std::string name)
{
	SetName(name);
	type = MATERIAL;
	myresourceID = uid; 

	// update reference counting in resource
	App->resources->UpdateResourceReferenceCount(myresourceID, 1);
}

ComponentMaterial::~ComponentMaterial()
{

}

void ComponentMaterial::CleanUp()
{
	// update reference counting in resource
	App->resources->UpdateResourceReferenceCount(myresourceID, -1);
}

textureData* ComponentMaterial::GetTextureData() const {
	return dynamic_cast<ResourceTexture*>(App->resources->Get(myresourceID))->GetTextureData(); 
}

ResourceTexture* ComponentMaterial::GetResourceTexture() const {
	return dynamic_cast<ResourceTexture*>(App->resources->Get(myresourceID)); 
}