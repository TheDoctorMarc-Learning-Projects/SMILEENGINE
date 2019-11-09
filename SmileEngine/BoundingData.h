#pragma once

#include <array>
#include "MathGeoLib/include/Math/float3.h"

namespace bounding
{
	struct BoundingBox
	{
		std::array<float3, 8> vertices;
		std::array<float, minMaxCoords::TOTAL_COORDS> minmaxCoords;
		float3 center;
		double boundingSphereRadius = 0;

		// debug
		std::array<bool, 8> insideoutside; // debug by frustrum
	};
}


// To be identified and treated differently while containing the same data
//typedef bounding::BoundingBox bounding::BoundingBox;

