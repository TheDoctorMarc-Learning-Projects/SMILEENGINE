#include "ResourceMeshPlane.h"
#include "Glew/include/GL/glew.h" 
#include "ResourceTexture.h"

ResourceMeshPlane::ResourceMeshPlane(SmileUUID uuid, ownMeshType type, std::string path) : ResourceMesh(uuid, type, path)
{
	GenerateOwnMeshData();
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
void ResourceMeshPlane::BlitMeshHere(float4x4& global_transform, ResourceTexture* tex, blendMode blendMode, float transparency)
{
	glPushMatrix();
	glMultMatrixf(global_transform.Transposed().ptr());

	// Cient states
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnable(GL_ALPHA_TEST);

	if (tex)
	{
		glBindTexture(GL_TEXTURE_2D, tex->GetTextureData()->id_texture);
		// Alpha Testing
		glAlphaFunc(GL_GREATER, (GLclampf)tex->GetTextureData()->transparency = transparency);
	}

	glEnable(GL_BLEND);
	if (blendMode == blendMode::ADDITIVE)
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	else if (blendMode == blendMode::ALPHA_BLEND)
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	glBegin(GL_QUADS);
	for (int i = 0; i < own_mesh->points.size(); i += 2)
	{
		if (tex && !own_mesh->uvCoords.empty())
			glTexCoord2f(own_mesh->uvCoords.at(i), own_mesh->uvCoords.at(i + 1));

		glVertex2f(own_mesh->points.at(i), own_mesh->points.at(i + 1));
	}
	glEnd();

	if (tex)
		glBindTexture(GL_TEXTURE_2D, 0);


	// Disable Cient states && clear data
	glColor3f(1.0f, 1.0f, 1.0f);
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