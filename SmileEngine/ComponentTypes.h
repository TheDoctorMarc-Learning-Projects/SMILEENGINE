#pragma once

#include <map>
#include <tuple>

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
