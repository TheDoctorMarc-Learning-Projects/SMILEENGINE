#pragma once

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"
#include "SmileModule.h"
#include "parshapes/par_shapes.h"
#pragma comment (lib, "Assimp/libx86/assimp.lib")

#include <list>

#include "DevIL/include/IL/il.h"

#include "glmath.h"


class Mesh { // TODO: do this generically with a template  	

public: 
	Mesh() {};
	~Mesh() {};

	uint id_index = 0;
	uint num_index = 0;
	uint* index = nullptr;

	uint num_normals = 0;
	uint id_normals = 0;
	float* normals = nullptr;

	uint id_vertex = 0;
	uint num_vertex = 0;
	float* vertex = nullptr;

	uint id_color = 0;
	uint num_color = 0;
	float* color = nullptr;

	uint id_UVs = 0;
	uint num_UVs = 0;
	float* UVs = nullptr;

	uint id_texture = 0;
	ILubyte* texture = nullptr;

	// AABB
private: 
	enum minMaxCoords : uint
	{
		MIN_X,
		MIN_Y,
		MIN_Z,
		MAX_X,
		MAX_Y,
		MAX_Z,
		TOTAL_COORDS
	};

	float minmaxCoords[minMaxCoords::TOTAL_COORDS]; 

	vec3 meshCenter; 
	double meshBoundingSphereRadius; 

	void ComputeMeshSpatialData()
	{
		// center 
		float c_X = (minmaxCoords[minMaxCoords::MIN_X] + minmaxCoords[minMaxCoords::MAX_X]) / 2;
		float c_Y = (minmaxCoords[minMaxCoords::MIN_Y] + minmaxCoords[minMaxCoords::MAX_Y]) / 2;
		float c_Z = (minmaxCoords[minMaxCoords::MIN_Z] + minmaxCoords[minMaxCoords::MAX_Z]) / 2;
		meshCenter = vec3(c_X, c_Y, c_Z);

		// sphere radius = module of -> (distance between opposite vertices) / 2
		vec3 min_Vec(minmaxCoords[minMaxCoords::MIN_X], minmaxCoords[minMaxCoords::MIN_Y], minmaxCoords[minMaxCoords::MIN_Z]); 
		vec3 max_Vec(minmaxCoords[minMaxCoords::MAX_X], minmaxCoords[minMaxCoords::MAX_Y], minmaxCoords[minMaxCoords::MAX_Z]); 
		vec3 rad_Vec = (max_Vec - min_Vec) / 2;
		meshBoundingSphereRadius = sqrt(rad_Vec.x * rad_Vec.x + rad_Vec.y * rad_Vec.y + rad_Vec.y * rad_Vec.y); 
	};

public:

	vec3 GetMeshCenter() const { return meshCenter; }; 
	double GetMeshSphereRadius() const { return meshBoundingSphereRadius; };

	friend class SmileFBX; 
};

struct FBX { // TODO: make this become a GameObject 
	std::vector<Mesh*> meshes;
};

class SmileFBX : public SmileModule
{
public:
	SmileFBX(SmileApp* app, bool start_enabled = true);
	~SmileFBX();
	bool Start();
	bool CleanUp();
	void ReadFBXData(const char* path);
	void DrawMesh(Mesh* mesh);
	void FreeMeshBuffers(Mesh* mesh); 
	void AssignTextureImageToMesh(const char* path, Mesh* mesh); 
	
};

