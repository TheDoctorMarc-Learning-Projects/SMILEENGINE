#pragma once

#include <string>
#include "SmileSetup.h"

class Resource
{
public: 
	enum Type { MESH, TEXTURE, SCENE, NO_TYPE };
public: 
	Resource::Resource(SmileUUID id);
	virtual Resource::~Resource() {};

	Resource::Type GetType() const { return type; };
	SmileUUID GetUID() const { return uid; };
	uint GetReferenceCount() const { return referenceCount; };
	const char* GetPath() const { return filePath.c_str();};
	virtual bool Load() {};
	virtual bool Save() {};

protected: 
	SmileUUID uid = 0;
	uint referenceCount = 0; 
	std::string filePath; 
	Type type = Type::NO_TYPE;  
};