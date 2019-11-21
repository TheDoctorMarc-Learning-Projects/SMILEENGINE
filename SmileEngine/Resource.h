#pragma once

#include <string>
#include "SmileSetup.h"

enum Resource_Type { 
	RESOURCE_MESH,
	RESOURCE_TEXTURE,
	RESOURCE_NO_TYPE
};

class Resource
{

public: 
	Resource(SmileUUID id, Resource_Type type) {};
	virtual ~Resource() {};

	Resource_Type GetType() const { return type; };
	SmileUUID GetUID() const { return uid; };
	uint GetReferenceCount() const { return referenceCount; };
	const char* GetPath() const { return filePath.c_str();};
	const char* GetImportedPath() const { return imported_filePath.c_str();};
	virtual bool Load() { return true; };
	virtual bool Save() { return true; };

	void SetFile(std::string file) { this->filePath = file; };
	void SetImportedFile(std::string imported_filePath) { this->imported_filePath = imported_filePath; };

protected: 
	SmileUUID uid = 0;
	uint referenceCount = 0; 
	std::string filePath; 
	std::string imported_filePath;
	Resource_Type type = Resource_Type::RESOURCE_NO_TYPE;
};