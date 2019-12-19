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
	FreeMemory(); 

	textureInfo = DBG_NEW textureData; 

	ILuint tempID;
	ilGenImages(1, &tempID);
	ilBindImage(tempID);
	ILboolean success = ilLoadImage(path); 

	if ((bool)success)
	{
		/*ILinfo img_info;
		iluGetImageInfo(&img_info);

		if (img_info.Origin != IL_ORIGIN_LOWER_LEFT)*/
			iluFlipImage();

		ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE);


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
			ilGetData());


		this->textureInfo->texture = ilGetData();
		this->textureInfo->width = (uint)ilGetInteger(IL_IMAGE_WIDTH);
		this->textureInfo->height = (uint)ilGetInteger(IL_IMAGE_HEIGHT);
		this->textureInfo->path = path;
	}
	else
		LOG("Error trying to load a texture image :( %s", iluErrorString(ilGetError()));


	glBindTexture(GL_TEXTURE_2D, 0);
	ilDeleteImages(1, &tempID);
}


void ResourceTexture::LoadCheckersOnMemory()
{
#ifndef CHECKERS_SIZE
#define CHECKERS_SIZE 20
#endif

	textureInfo = DBG_NEW textureData;
	GLubyte checkImage[CHECKERS_SIZE][CHECKERS_SIZE][4];
	for (uint i = 0; i < CHECKERS_SIZE; i++)
	{
		for (uint j = 0; j < CHECKERS_SIZE; j++)
		{
			uint c = ((((i & 0x8) == 0) ^ (((j & 0x8)) == 0))) * 255;
			checkImage[i][j][0] = (GLubyte)c;
			checkImage[i][j][1] = (GLubyte)c;
			checkImage[i][j][2] = (GLubyte)c;
			checkImage[i][j][3] = (GLubyte)255;
		}
	}

	ILinfo img_info;
	iluGetImageInfo(&img_info);

	if (img_info.Origin != IL_ORIGIN_LOWER_LEFT)
		iluFlipImage();

	ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);

	glGenTextures(1, (GLuint*)&textureInfo->id_texture);
	glBindTexture(GL_TEXTURE_2D, (GLuint)textureInfo->id_texture);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (GLuint)CHECKERS_SIZE,
		(GLuint)CHECKERS_SIZE, 0, GL_RGBA, GL_UNSIGNED_BYTE,
		checkImage);

	textureInfo->texture = (ILubyte*)checkImage;
	textureInfo->path = "Generated in-game";

	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void ResourceTexture::FreeMemory()
{
	if (textureInfo == nullptr)
		return; 

	if (textureInfo->texture != nullptr)
	{
		glDeleteTextures(1, (GLuint*)&textureInfo->texture);
		//delete[] mesh->texture; 
		textureInfo->id_texture = 0;
	}

	RELEASE(textureInfo);
}
