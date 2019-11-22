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

