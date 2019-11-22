#include "ResourceTexture.h"
#include "Glew/include/GL/glew.h"

void ResourceTexture::FreeMemory()
{
	if (textureInfo->texture != nullptr)
	{
		glDeleteTextures(1, (GLuint*)&textureInfo->texture);
		//delete[] mesh->texture; 
		textureInfo->id_texture = 0;
	}

	RELEASE(textureInfo);
}
 