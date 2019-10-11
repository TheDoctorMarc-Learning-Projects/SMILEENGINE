#include "SmileApp.h"
#include "SmileFBX.h"
#include "Glew/include/GL/glew.h" 
#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"
#pragma comment (lib, "Assimp/libx86/assimp.lib")

#include "DevIL/include/IL/ilu.h"
#include "DevIL/include/IL/ilut.h"

#pragma comment (lib, "DevIL/libx86/DevIL.lib")
#pragma comment (lib, "DevIL/libx86/ILU.lib")
#pragma comment (lib, "DevIL/libx86/ILUT.lib")

SmileFBX::SmileFBX(SmileApp* app, bool start_enabled) : SmileModule(app, start_enabled) 
{

}

SmileFBX::~SmileFBX() 
{}

bool SmileFBX::Start()
{
	bool ret = true;
	struct aiLogStream stream;
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	aiAttachLogStream(&stream);

	// Devil
	ilInit(); 
	iluInit(); 
	ilutRenderer(ILUT_OPENGL); 

	return ret;
}

bool SmileFBX::CleanUp()
{
	aiDetachAllLogStreams();
	return true;
}

void SmileFBX::ReadFBXData(const char* path) {

	const aiScene* scene = aiImportFile(path, aiProcessPreset_TargetRealtime_MaxQuality);

	if (scene != nullptr && scene->HasMeshes()) 
	{
		FBX fbx_info; 
		for (int i = 0; i < scene->mNumMeshes; ++i) 
		{

			// Vertexs
			aiMesh* new_mesh = scene->mMeshes[i];
			Mesh mesh_info; 
			mesh_info.num_vertex = new_mesh->mNumVertices;
			mesh_info.vertex = new float[mesh_info.num_vertex * 3];
			memcpy(mesh_info.vertex, new_mesh->mVertices, sizeof(float) * mesh_info.num_vertex * 3);
			LOG("New Mesh with %d vertices", mesh_info.num_vertex);

		    // Indexes
			if (new_mesh->HasFaces())
			{
				mesh_info.num_index = new_mesh->mNumFaces * 3;
				mesh_info.index = new uint[mesh_info.num_index];
				for(uint i = 0; i< new_mesh->mNumFaces; ++i)
				{
					if (new_mesh->mFaces[i].mNumIndices != 3)
					{
						LOG("WARNING, geometry face with != 3 indices!");
					}
					else
					{
						memcpy(&mesh_info.index[i * 3], new_mesh->mFaces[i].mIndices, 3 * sizeof(float));
					}
					
				}
			
			}

			// Normals
			if (new_mesh->HasNormals())
			{
				mesh_info.num_normals = new_mesh->mNumVertices;
				mesh_info.normals = new float[mesh_info.num_vertex * 3];
				memcpy(mesh_info.normals, new_mesh->mNormals, sizeof(float) * mesh_info.num_normals * 3);

			}

			// UVs
			for (int i = 0; i < new_mesh->GetNumUVChannels(); ++i)
			{
				if (new_mesh->HasTextureCoords(i))
				{
					mesh_info.num_UVs = new_mesh->mNumVertices;
					mesh_info.UVs = new float[mesh_info.num_UVs * 2];
					memcpy(mesh_info.UVs, (float*)new_mesh->mTextureCoords[i], sizeof(float) * 2);
				}

			}


			// colors 
			if (new_mesh->GetNumColorChannels() > 0)
			{
				for (int i = 0; i < new_mesh->GetNumColorChannels(); ++i)
				{
					if (new_mesh->HasVertexColors(i))
					{
						mesh_info.num_color = new_mesh->mNumVertices;
						mesh_info.color = new float[mesh_info.num_color * 3];  // TODO: do not assume RBG, may be RGBA (xd)  
						memcpy(mesh_info.color, new_mesh->mColors[i], sizeof(float) * mesh_info.num_color * 3);
					}
				
				}
			}

			// Normals Buffer
			glGenBuffers(1, (GLuint*) & (mesh_info.id_normals));
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh_info.id_normals);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(float) * mesh_info.num_normals * 3, mesh_info.normals, GL_STATIC_DRAW);
			
			// Uvs vBuffer
			glGenBuffers(1, (GLuint*) & (mesh_info.id_UVs));
			glBindBuffer(GL_ARRAY_BUFFER, mesh_info.id_UVs);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh_info.num_UVs * 2, mesh_info.UVs, GL_STATIC_DRAW);

			// Color Buffer
			glGenBuffers(1, (GLuint*) & (mesh_info.id_color));
			glBindBuffer(GL_ARRAY_BUFFER, mesh_info.id_color);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh_info.num_color * 3, mesh_info.color, GL_STATIC_DRAW);
		
			// Vertex Buffer
			glGenBuffers(1, (GLuint*) & (mesh_info.id_vertex));
			glBindBuffer(GL_ARRAY_BUFFER, mesh_info.id_vertex);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh_info.num_vertex * 3, mesh_info.vertex, GL_STATIC_DRAW);

			// Index Buffer
			glGenBuffers(1, (GLuint*) & (mesh_info.id_index));
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh_info.id_index);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * mesh_info.num_index, mesh_info.index, GL_STATIC_DRAW);

			

			fbx_info.meshes.push_back(mesh_info); 
			App->scene_intro->fbxs.push_back(fbx_info);
		}
		
		aiReleaseImport(scene);
	}
	else
	{
		LOG("Error loading FBX %s", path);
	}
}

void SmileFBX::DrawMesh(Mesh& mesh) 	// TODO: textureeeeeessss
{
	glColor3f(0.3f, 0.3f, 0.3f);

	// Cient states
	glEnableClientState(GL_VERTEX_ARRAY); 
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	//glEnableClientState(GL_TEXTURE_BUFFER);
	//glEnableClientState(GL_COLOR_ARRAY); 

	// UV buffer
	if (mesh.UVs != nullptr)
	{
		glBindBuffer(GL_ARRAY_BUFFER, mesh.id_UVs);
		glTexCoordPointer(2, GL_FLOAT, 0, NULL);

	}

	// color buffer 
	/*if (mesh.color != nullptr) {
		glBindBuffer(GL_COLOR_ARRAY, mesh.id_color);
		glColorPointer(3, GL_FLOAT, 8 * sizeof(GLfloat), 0);
	}*/

    // texture buffer
	if (mesh.texture != nullptr)
		glBindTexture(GL_TEXTURE_2D, mesh.id_texture);


	// normal buffer
	if (mesh.normals != nullptr)
	{
		glBindBuffer(GL_NORMAL_ARRAY, mesh.id_normals);
		glNormalPointer(GL_FLOAT, 3, NULL);
	}

	// vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, mesh.id_vertex);
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	// index buffer 
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.id_index);
	glDrawElements(GL_TRIANGLES, mesh.num_index * 3, GL_UNSIGNED_INT, NULL);


	// Cient states
	//glDisableClientState(GL_COLOR_ARRAY);
	//glDisableClientState(GL_TEXTURE_BUFFER);
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);


	// draw normals
	if (mesh.normals != nullptr)
	{
		glColor3f(0.f, 1.0f, 0.f);
		static float normalFactor = 20.f;

		for (int i = 0; i < mesh.num_normals * 3; i += 3)
		{
			glBegin(GL_LINES);

			vec3 normalVec = normalize({ mesh.normals[i], mesh.normals[i + 1], mesh.normals[i + 2] });
			glVertex3f(mesh.vertex[i], mesh.vertex[i + 1], mesh.vertex[i + 2]);
			glVertex3f(mesh.vertex[i] + normalVec.x, mesh.vertex[i + 1] + normalVec.y, mesh.vertex[i + 2] + normalVec.z);

			glEnd();
		}
	}


}

void SmileFBX::FreeMeshBuffers(Mesh& mesh)
{
	if (mesh.vertex != nullptr)
	{
		glDeleteBuffers(1, (GLuint*)& mesh.vertex);
		delete[] mesh.vertex;
	}

	if (mesh.index != nullptr)
	{
		glDeleteBuffers(1, (GLuint*)& mesh.index);
		delete[] mesh.index;
	}
	
	if (mesh.normals != nullptr)
	{
		glDeleteBuffers(1, (GLuint*)& mesh.normals);
		delete[] mesh.normals;
	}
	
	if (mesh.color != nullptr)
	{
		glDeleteBuffers(1, (GLuint*)& mesh.color);
		delete[] mesh.color;
	}

	if (mesh.UVs != nullptr)
	{
		glDeleteBuffers(1, (GLuint*)& mesh.UVs);
		delete[] mesh.UVs;
	}
		
	if (mesh.texture != nullptr)
	{
		glDeleteTextures(1, (GLuint*)& mesh.texture);
		//delete[] mesh.texture; 
	}
	

}

// TODO: somehow know beforehand to which mesh the cursor dropped the texture file into, and pass it here
void SmileFBX::AssignTextureImageToMesh(const char* path, Mesh& mesh)
{	 
	// Devil stuff
	ilGenImages(1, &(ILuint)mesh.id_texture);
	ilBindImage((ILuint)mesh.id_texture);

	path = "..//Assets/Images/Lenna.png";  // overwrite for the mom in the case of drag & drop the texture .png
	ILboolean success = ilLoadImage(path);
	ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE); 

	static ILuint Width = ilGetInteger(IL_IMAGE_WIDTH);
	static ILuint Height = ilGetInteger(IL_IMAGE_HEIGHT);
	mesh.texture = ilGetData(); 

	// TODO: get GL_RGB or GL_RGBA properly (.pngs do not have the A value) 

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, (GLuint*)& mesh.id_texture);
	glBindTexture(GL_TEXTURE_2D, (GLuint)mesh.id_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, ilGetInteger(IL_IMAGE_FORMAT), (GLuint)Width, (GLuint)Height,
		0, GL_RGB, GL_UNSIGNED_BYTE, mesh.texture);
	glGenerateMipmap(GL_TEXTURE_2D);


	ilDeleteImage((ILuint)mesh.id_texture);
}
