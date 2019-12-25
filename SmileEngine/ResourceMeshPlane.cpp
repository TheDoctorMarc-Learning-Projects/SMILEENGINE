#include "ResourceMeshPlane.h"
#include "Glew/include/GL/glew.h" 
#include "ResourceTexture.h"

ResourceMeshPlane::ResourceMeshPlane(SmileUUID uuid, ownMeshType type, std::string path, float4 color, TileData* tileData) : ResourceMesh(uuid, type, path), color(color), tileData(tileData)
{
	GenerateOwnMeshData();
	/*if(color.IsFinite())
		LoadOnMemory(color);*/
}

void ResourceMeshPlane::LoadOnMemory(float4 color)
{
	

	bufferData.num_color = 4;
	bufferData.color = new float[bufferData.num_color * 4];
	uint j = 0;
	for (uint i = 0; i < 4; ++i)
	{
		memcpy(&bufferData.color[j], &color.x, sizeof(float));
		memcpy(&bufferData.color[j + 1], &color.y, sizeof(float));
		memcpy(&bufferData.color[j + 2], &color.z, sizeof(float));
		memcpy(&bufferData.color[j + 3], &color.w, sizeof(float));
		j += 4;
	}

	// Color Buffer
	glGenBuffers(1, (GLuint*) & (bufferData.id_color));
	glBindBuffer(GL_ARRAY_BUFFER, bufferData.id_color);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * bufferData.num_color * 4, bufferData.color, GL_STATIC_DRAW);
}

void ResourceMeshPlane::FreeMemory()
{
	if(tileData)
		RELEASE(tileData);

	/*glDeleteBuffers(1, (GLuint*)&bufferData.color);
	RELEASE_ARRAY(bufferData.color);*/
}

void ResourceMeshPlane::GenerateOwnMeshData()
{
	own_mesh = DBG_NEW ownMeshData;
	own_mesh->type = ownMeshType::plane;
	own_mesh->size = 0.5f;
	own_mesh->points = { -own_mesh->size / 2, own_mesh->size/2, -own_mesh->size / 2, -own_mesh->size / 2, own_mesh->size / 2, -own_mesh->size / 2, own_mesh->size / 2, own_mesh->size / 2 };
	own_mesh->points3D = { -own_mesh->size / 2, own_mesh->size / 2, 0, -own_mesh->size / 2, -own_mesh->size / 2, 0, own_mesh->size / 2, -own_mesh->size / 2, 0, own_mesh->size / 2, own_mesh->size / 2, 0 };
    own_mesh->uvCoords = { 0,1,0,0,1,0,1,1 };

	// Any buffer? Color? Normals? 
}

// TODO: blend mode
void ResourceMeshPlane::BlitMeshHere(float4x4& global_transform, ResourceTexture* tex, blendMode blendMode, float transparency, float4 color, uint tileIndex)
{
	glPushMatrix();
	glMultMatrixf(global_transform.Transposed().ptr());

	// Cient states
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);


	// Texture
	if (tex)
	{
		glEnable(GL_ALPHA_TEST);

		glBindTexture(GL_TEXTURE_2D, tex->GetTextureData()->id_texture);
		// Alpha Testing
		glAlphaFunc(GL_GREATER, (GLclampf)tex->GetTextureData()->transparency = transparency);
	}
	else
	{
		// Color Blending
		glEnable(GL_BLEND);
		if (blendMode == blendMode::ADDITIVE)
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		else if (blendMode == blendMode::ALPHA_BLEND)
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	}

	// Color
	if (color.IsFinite())
	{
		this->color = color; 
		glColor4f(this->color.x, this->color.y, this->color.z, this->color.w);
	}
	

	// Geometry
	glBegin(GL_QUADS);
	for (int i = 0; i < own_mesh->points.size(); i += 2)
	{
		if (tex && !own_mesh->uvCoords.empty())
		{
			if (tileIndex != INFINITE)
				UpdateTileUvs(tileIndex);

			glTexCoord2f(own_mesh->uvCoords.at(i), own_mesh->uvCoords.at(i + 1));
		}
			

		glVertex2f(own_mesh->points.at(i), own_mesh->points.at(i + 1));
	}
	glEnd();

	if (tex)
		glBindTexture(GL_TEXTURE_2D, 0);


	// Disable Cient states && clear data
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glAlphaFunc(GL_EQUAL, (GLclampf)1.f);
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_BLEND); 
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);


		// Transformation
	glPopMatrix();
}

void ResourceMeshPlane::UpdateTileUvs(uint tileIndex)
{
	// todo: know current row and column from index
	uint row = tileIndex / tileData->nCols; 
	uint col = tileIndex % tileData->nCols; 

	float sizeX = 1 / (float)(int)tileData->nCols; 
	float sizeY = 1 / (float)(int)tileData->nRows;

	own_mesh->uvCoords = { col * sizeX, row * sizeY, col * sizeX, (row + 1) * sizeY,
		(col + 1) * sizeX, (row + 1) * sizeY, (col + 1) * sizeX, row * sizeY};
}