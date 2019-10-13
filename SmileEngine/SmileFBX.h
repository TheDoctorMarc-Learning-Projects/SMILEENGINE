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

#include "ComponentMesh.h"

class SmileFBX : public SmileModule
{
public:
	SmileFBX(SmileApp* app, bool start_enabled = true);
	~SmileFBX();
	bool Start();
	bool CleanUp();
	void ReadFBXData(const char* path);
public:
	bool debug = false;
	
};

