#include "ComponentMaterial.h"
#include "Glew/include/GL/glew.h"

ComponentMaterial::ComponentMaterial()
{
	SetName("Material");
	type = MATERIAL;
	textureInfo = DBG_NEW textureData; 
}

ComponentMaterial::~ComponentMaterial()
{

}

void ComponentMaterial::CleanUp()
{
	// TODO: pass resource id, add 1 in constructor, and substract 1 here 
}

