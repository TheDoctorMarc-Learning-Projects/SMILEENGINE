#pragma once

#include <map>
#include <tuple>
enum COMPONENT_TYPE : unsigned int
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

std::map<COMPONENT_TYPE, std::tuple<bool, bool, bool>> uniquenessMap;    // to access use std::get with 0, 1, 2 for the bools
