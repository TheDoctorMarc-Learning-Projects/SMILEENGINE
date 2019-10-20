#include "ComponentMaterial.h"
#include "Glew/include/GL/glew.h"

ComponentMaterial::ComponentMaterial()
{
	type = MATERIAL;
	textureInfo = DBG_NEW textureData; 
}

ComponentMaterial::~ComponentMaterial()
{

}

void ComponentMaterial::CleanUp()
{
	if (textureInfo->texture != nullptr)
	{
		glDeleteTextures(1, (GLuint*)&textureInfo->texture);
		//delete[] mesh->texture; 
	}

	RELEASE(textureInfo); 
}
