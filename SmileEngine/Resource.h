#pragma once

#include <string>
#include "SmileSetup.h"

enum Resource_Type { 
	RESOURCE_MESH,
	RESOURCE_TEXTURE,
	RESOURCE_SKYBOX,
	RESOURCE_NO_TYPE
};

class Resource
{

public: 
	Resource(SmileUUID id, Resource_Type type, std::string filePath) : uid(id), type(type), filePath(filePath) {};
	virtual ~Resource() {};

	Resource_Type GetType() const { return type; };
	SmileUUID GetUID() const { return uid; };
	SmileUUID GetUID() { return uid; };
	uint GetReferenceCount() const { return referenceCount; };
	const char* GetPath() const { return filePath.c_str();};
	const char* GetImportedPath() const { return imported_filePath.c_str();};
	
	virtual void LoadOnMemory(const char* path = { 0 }) {};
	virtual void FreeMemory() {};

	void SetFile(std::string file) { this->filePath = file; };
	void SetImportedFile(std::string imported_filePath) { this->imported_filePath = imported_filePath; };

	void SetPreset(bool preset) { this->preset = preset; }; 
	bool IsPreset() const { return preset; };
protected: 
	bool preset = false; // will be used eg by primitives, they should always be available (not delete when refs = 0)
	SmileUUID uid = 0;
	uint referenceCount = 0; 
	std::string filePath; 
	std::string imported_filePath;
	Resource_Type type = Resource_Type::RESOURCE_NO_TYPE;

	friend class SmileResourceManager; 
};