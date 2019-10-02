#ifndef __GeometryGenerator_H__
#define __GeometryGenerator_H__

#ifndef ACCESS_TO_GEOMETRY
#error This file is for inclusion in the gui module only
#endif

#include "MathGeoLib/include/MathGeoLib.h"

#ifdef NDEBUG //no debug
#pragma comment (lib, "MathGeoLib/libx86/ReleaseLib/MathGeoLib.lib") 
#else
#pragma comment (lib, "MathGeoLib/libx86/DebugLib/MathGeoLib.lib") 
#endif

#include <unordered_map>
#include <locale>
#include <stdarg.h>
#include <typeinfo>

#include "SmileApp.h"

namespace GeometryGenerator
{
	// -----------------------------------------------------------------
	std::unordered_map<std::string, math::GeomType> typeMap;
	std::unordered_map<std::string, std::pair<int, std::vector<std::string>>> parameterMap;  // name, num of params, name of params

	void PopulateMap(bool create = true)
	{
		if (create == true) // TODO: keep populating the maps
		{
			typeMap =
			{
				{"Capsule", math::GeomType::GTCapsule},
				{"Circle", math::GeomType::GTCircle},
			};

			std::vector<std::string> capsuleParam = { "TopX", "TopY", "TopZ", "BottomX", "BottomY", "BottomZ", "Radius" }; 
			parameterMap =
			{
				{"Capsule", std::pair(7, capsuleParam)},
			};

		}
		else
		{
			typeMap.clear();
			for (auto& x : parameterMap)
				x.second.second.clear();
			parameterMap.clear(); 
		}
			

	};

	// ----------------------------------------------------------------- [See if an associated object type exists in the map]
	bool DoesObjectExist(std::string name)
	{
		auto type = typeMap.find(name);
		if (type != typeMap.end())
			return true; 
		return false; 
	}

	// ----------------------------------------------------------------- [Get the parameter string vector for a given object name]
	std::vector<std::string> GetObjectParameters(std::string name)
	{
		auto item = parameterMap.find(name);
		if (item != parameterMap.end())
			return (*item).second.second; 
	
		std::vector<std::string> error = { "error" };

		return error; 
	}

	// ----------------------------------------------------------------- [Get the number of arguments for a given object name]
	int GetObjectParameterCount(std::string name)
	{
		auto item = parameterMap.find(name);
		if (item != parameterMap.end())
			return (*item).second.first; 
		return (int)NAN; 
	}

	// ----------------------------------------------------------------- [Generate any geometry object given a valid name and arguments] 
	void GenerateObject(std::string name, int n_args, ...) // pass this function arguments ORDERED to call a particular object constructor: n_args = name + n_args + object params
	{
		// 1) find if there exists an object type with the name
		auto type = typeMap.find(name);
		if (type == typeMap.end())
		{
			if (isupper(name[0]) == false)
				if (App)
					App->gui->Log("The object name must begin with upper case");
			return;
		}
		math::GeomType wantedType = typeMap.at(name);

		// 2) Retrieve the data. 
		// It HAS to be floats passed by order. It can't be "math::float3" because "va_arg" won't accept it as argument
		std::vector<float> data;
		va_list ap;

		va_start(ap, n_args);
		for (int i = 3; i <= n_args; ++i)
		{
			float variable = (float)va_arg(ap, double);   
			data.push_back(variable);
		}
		va_end(ap);

		if (data.size() != GetObjectParameterCount(name))
			return; 

		// 3) Generate the object -> use a pre-selected constructor with the previous data: they have to match
		switch (wantedType)
		{
			/*case math::GTPoint:
				break;
			case math::GTAABB:
				break;*/
		case math::GTCapsule:
			math::Capsule(math::float3(data.at(0), data.at(1), data.at(2)), math::float3(data.at(3), data.at(4), data.at(5)), data.at(6));
			break;
		case math::GTCircle:
			break;
		case math::GTFrustum:
			break;
		case math::GTLine:
			break;
		case math::GTLineSegment:
			break;
		case math::GTOBB:
			break;
		case math::GTPlane:
			break;
		case math::GTPolygon:
			break;
		case math::GTPolyhedron:
			break;
		case math::GTRay:
			break;
		case math::GTSphere:
			break;
		case math::GTTriangle:
			break;
		default:
			break;
		}

		data.clear();
	}; 

}

  
#endif // __GeometryGenerator_H__



