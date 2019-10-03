#ifndef __GeometryGenerator_H__
#define __GeometryGenerator_H__

#ifndef ACCESS_TO_GEOMETRY
#error This file is for inclusion in the gui module only
#endif 

#undef ACCESS_TO_GEOMETRY 

#include "MathGeoLib/include/MathGeoLib.h"

#ifdef NDEBUG //no debug
#pragma comment (lib, "MathGeoLib/libx86/ReleaseLib/MathGeoLib.lib") 
#else
#pragma comment (lib, "MathGeoLib/libx86/DebugLib/MathGeoLib.lib") 
#endif

#include "parshapes/par_shapes.h"

#include <unordered_map>
#include <locale>
#include <stdarg.h>
#include <typeinfo>

#include "SmileApp.h"

namespace GeometryGenerator
{
	namespace MathGeoLib
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
					{"Line", math::GeomType::GTLine},
					{"Line Segment", math::GeomType::GTLineSegment},
					{"Plane", math::GeomType::GTPlane},
					{"Ray", math::GeomType::GTRay},
					{"Sphere", math::GeomType::GTSphere},
					{"Triangle", math::GeomType::GTTriangle},
				};

				std::vector<std::string> capsuleParam = { "TopX", "TopY", "TopZ", "BottomX", "BottomY", "BottomZ", "Radius" };
				std::vector<std::string> circleParam = { "CenterX", "CenterY", "CenterZ", "NormalX", "NormalY", "NormalZ", "Radius" };
				std::vector<std::string> lineParam = { "PositionX", "PositionY", "PositionZ", "DirectionX", "DirectionY", "DirectionZ" };
				std::vector<std::string> lineSegmentParam = { "PointA_X", "PointA_Y", "PointA_Z", "PointB_X", "PointB_Y", "PointB_Z" };
				std::vector<std::string> planeParam = { "PointX", "PointY", "PointZ", "NormalX", "NormalY", "NormalZ" };
				std::vector<std::string> rayParam = lineParam;
				std::vector<std::string> sphereParam = lineSegmentParam;
				std::vector<std::string> triangleSegmentParam = { "PointA_X", "PointA_Y", "PointA_Z", "PointB_X", "PointB_Y", "PointB_Z", "PointC_X", "PointC_Y", "PointC_Z" };

				parameterMap =
				{
					{"Capsule", std::pair(7, capsuleParam)},
					{"Circle", std::pair(7, circleParam)},
					{"Line", std::pair(6, lineParam)},
					{"Line Segment", std::pair(6, lineSegmentParam)},
					{"Plane", std::pair(6, planeParam)},
					{"Ray", std::pair(6, rayParam)},
					{"Sphere", std::pair(6, sphereParam)},
					{"Triangle", std::pair(9, triangleSegmentParam)},
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

		void GetAllObjectTypesChar(char(&array)[128])
		{
			strcpy(array, "Available types:");
			strcat(array, "\n");

			for (auto& string : typeMap)
			{
				strcat(array, string.first.c_str());
				strcat(array, "\n");
			}
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
		void GenerateObject(std::string name, std::vector<float> data) // pass this function arguments ORDERED to call a particular object constructor
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

			if (data.size() != GetObjectParameterCount(name))
				return;

			// 2) Generate the object -> use a pre-selected constructor with the previous data: they have to match

			// TODO: normalize line dir and ray dir
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
				math::Circle(math::float3(data.at(0), data.at(1), data.at(2)), math::float3(data.at(3), data.at(4), data.at(5)), data.at(6));
				break;
				/*	case math::GTFrustum:
						break;*/
			case math::GTLine:
				math::Line(math::float3(data.at(0), data.at(1), data.at(2)), math::float3(data.at(3), data.at(4), data.at(5)));
				break;
			case math::GTLineSegment:
				math::LineSegment(math::float3(data.at(0), data.at(1), data.at(2)), math::float3(data.at(3), data.at(4), data.at(5)));
				break;
				/*case math::GTOBB:
					break;*/
			case math::GTPlane:
				math::Plane(math::float3(data.at(0), data.at(1), data.at(2)), math::float3(data.at(3), data.at(4), data.at(5)));
				break;
				/*case math::GTPolygon:
					break;
				case math::GTPolyhedron:
					break;*/
			case math::GTRay:
				math::Ray(math::float3(data.at(0), data.at(1), data.at(2)), math::float3(data.at(3), data.at(4), data.at(5)));
				break;
			case math::GTSphere:
				math::Sphere(math::float3(data.at(0), data.at(1), data.at(2)), math::float3(data.at(3), data.at(4), data.at(5)));
				break;
			case math::GTTriangle:
				math::Triangle(math::float3(data.at(0), data.at(1), data.at(2)), math::float3(data.at(3), data.at(4), data.at(5)), math::float3(data.at(6), data.at(7), data.at(8)));
				break;
			default:
				break;
			}

			data.clear();
		};
	}
	
	namespace ParShapes
	{
		/*par_shapes_mesh* TestFunction()
		{
	        return par_shapes_create_cube();
		}*/
	}

}

  
#endif // __GeometryGenerator_H__



