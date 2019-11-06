#pragma once
#include "SmileModule.h"
#include <string>
#include "ComponentTypes.h"

class SmileMaterialImporter: public SmileModule
{
public:
	SmileMaterialImporter(SmileApp* app, bool start_enabled = true);
	~SmileMaterialImporter() {};

	bool Import(const char* file, const char* path, std::string& output_file);
	bool Import(const void* buffer, uint size, std::string& output_file);
	bool Load(const char* exported_file, textureData* resource);
	bool LoadCheckers(textureData* resource);
};
