#pragma once

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"
#include "SmileModule.h"
#include "parshapes/par_shapes.h"
#pragma comment (lib, "Assimp/libx86/assimp.lib")

#include <list>


struct Mesh {
	uint id_index = 0;
	uint num_index = 0;
	uint* index = nullptr;

	uint num_normals = 0;
	uint id_normals = 0;
	float* normals = nullptr;

	uint id_vertex = 0;
	uint num_vertex = 0;
	float* vertex = nullptr;

	/*uint id_color = 0;
	uint num_color = 0;
	float* color = nullptr;*/

	uint id_UVs = 0;
	uint num_UVs = 0;
	float* UVs = nullptr;

	uint id_texture = 0;
	//const void* texture = nullptr; 
};

struct FBX {
	std::vector<Mesh> meshes;
};

class SmileFBX : public SmileModule
{
public:
	SmileFBX(SmileApp* app, bool start_enabled = true);
	~SmileFBX();
	bool Start();
	bool CleanUp();
	void ReadFBXData(const char* path);
	void DrawMesh(Mesh& mesh);
	void FreeMeshBuffers(Mesh& mesh); 
	void AssignTextureImageToMesh(const char* path, Mesh& mesh); 
	
};

