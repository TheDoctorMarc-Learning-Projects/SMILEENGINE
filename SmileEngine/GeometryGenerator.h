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
	std::unordered_map<std::string, math::GeomType> objectMapType;

	void PopulateMap(bool create = true)
	{
		if (create == true)
		{
			objectMapType =
			{
				{"Capsule", math::GeomType::GTCapsule},
				{"Circle", math::GeomType::GTCircle},
			};
		}
		else
			objectMapType.clear();

	};

	void GenerateObject(std::string name, int n_args, ...) // pass this function arguments ORDERED to call a particular object constructor
	{
		// 1) find if there exists an object type with the name
		auto type = objectMapType.find(name);
		if (type == objectMapType.end())
		{
			if (isupper(name[0]) == false)
				if (App)
					App->gui->Log("The object name must begin with upper case");
			return;
		}
		math::GeomType wantedType = objectMapType.at(name);

		// 2) Retrieve the data -> it HAS to be passed by order and it can't be math::float3 because it can't be passed to va_arg
		std::vector<float> data;
		va_list ap;

		va_start(ap, n_args);
		for (int i = 3; i <= n_args; ++i)
		{
			float variable = (float)va_arg(ap, double);  // TODO: this gets a 0 xd
			data.push_back(variable);
		}
		va_end(ap);


		// 3) Generate the object -> use a pre-selected constructor with the previous data

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



