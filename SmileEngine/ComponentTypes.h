#pragma once

#include <map>
#include <tuple>

enum COMPONENT_TYPE : uint
{
	TRANSFORM,
	MESH,
	MATERIAL,
	LIGHT,
	MAX_COMPONENT_TYPES,
	NO_TYPE
};

// 1st bool = can it have it the GameObject?
// 2nd bool = can it have it each mesh? 
// 3rd bool = can there be more than 1 instance per GameObject / Mesh?  

const std::map<COMPONENT_TYPE, std::tuple<bool, bool, bool>> uniquenessMap
=
{
		{ COMPONENT_TYPE::TRANSFORM, std::tuple(true, true, false) },
		{ COMPONENT_TYPE::MESH,  std::tuple(true, false, true) },
		{ COMPONENT_TYPE::MATERIAL,  std::tuple(false, true, false) },
		{ COMPONENT_TYPE::LIGHT,  std::tuple(true, false, false) },
}; 
// to access the tuple use std::get with 0, 1, 2 for each bool eg. std::get<0>(...)

 
#include "DevIL/include/IL/il.h"
typedef ILubyte;
struct textureData
{
	uint id_texture = 0;
	uint width, height = 0; 
	const char* path; 
	ILubyte* texture = nullptr;
};