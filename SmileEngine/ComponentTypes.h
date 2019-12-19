#pragma once

#include <map>

enum COMPONENT_TYPE : uint
{
	TRANSFORM,
	MESH,
	MATERIAL,
	LIGHT,
	CAMERA,
	MAX_COMPONENT_TYPES,
	NO_TYPE
};

#include "DevIL/include/IL/il.h"
typedef ILubyte;
struct textureData
{
	uint id_texture = 0;
	uint width, height = 0; 
	std::string path = "empty";
	ILubyte* texture = nullptr;
	std::string format = "empty"; 
	float transparency = 1.f; 
};

// AABBs
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

static std::map<std::string, COMPONENT_TYPE> componentTypeMap =
{
	{ "Transform", TRANSFORM},
	{ "Mesh", MESH},
	{ "Material", MATERIAL},
	{ "Camera", CAMERA},
}; 