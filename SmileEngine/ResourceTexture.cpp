#include "ResourceTexture.h"
#include "Glew/include/GL/glew.h"
#include "SmileApp.h"
#include "SmileResourceManager.h"

#include "DevIL/include/IL/ilu.h"
#include "DevIL/include/IL/ilut.h"

#pragma comment (lib, "DevIL/libx86/DevIL.lib")
#pragma comment (lib, "DevIL/libx86/ILU.lib")
#pragma comment (lib, "DevIL/libx86/ILUT.lib")

void ResourceTexture::LoadOnMemory(const char* path)
{
	textureInfo = DBG_NEW textureData; 

	ILuint tempID;
	ilGenImages(1, &tempID);
	ilBindImage(tempID);
	ILboolean success = ilLoadImage(path); 

	if ((bool)success)
	{
		ILinfo img_info;
		iluGetImageInfo(&img_info);

		if (img_info.Origin != IL_ORIGIN_LOWER_LEFT)
			iluFlipImage();

		ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE);

		this->textureInfo->texture = ilGetData();
		this->textureInfo->width = (uint)ilGetInteger(IL_IMAGE_WIDTH);
		this->textureInfo->height = (uint)ilGetInteger(IL_IMAGE_HEIGHT);
		this->textureInfo->path = path;

		glGenTextures(1, (GLuint*)&textureInfo->id_texture);
		glBindTexture(GL_TEXTURE_2D, (GLuint)textureInfo->id_texture);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		glTexImage2D(GL_TEXTURE_2D, 0, ilGetInteger(IL_IMAGE_FORMAT), (GLuint)ilGetInteger(IL_IMAGE_WIDTH),
			(GLuint)ilGetInteger(IL_IMAGE_HEIGHT), 0, ilGetInteger(IL_IMAGE_FORMAT), GL_UNSIGNED_BYTE,
			this->textureInfo->texture);
	}
	else
		LOG("Error trying to load a texture image :( %s", iluErrorString(ilGetError()));

	ilDeleteImages(1, &tempID);
}

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
 