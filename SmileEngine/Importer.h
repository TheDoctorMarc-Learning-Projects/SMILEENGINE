#pragma once

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"
#pragma comment (lib, "Assimp/libx86/assimp.lib")

#ifndef ACCESS_TO_IMPORTER
#error This file is for inclusion in the scene module only
#endif 

#undef ACCESS_TO_IMPORTER 

namespace importer
{
	const aiScene* OpenFBX(const char* path) { aiImportFile(path, aiProcessPreset_TargetRealtime_MaxQuality); };
	void ReleaseFBX(const aiScene*& scene) { if(scene && scene != nullptr) aiReleaseImport(scene);};
}

