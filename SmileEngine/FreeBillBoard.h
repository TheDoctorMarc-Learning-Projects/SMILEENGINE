#pragma once

#include "MathGeoLib/include/Math/float4x4.h"
#include "MathGeoLib/include/Math/float3.h"

#include <variant>

#include "FreeBillBoard.h"
#include "ComponentTransform.h"
#include "FreeTransform.h"

struct FreeBillBoard
{

public:
	enum Alignment { axis, screen, world, noAlignment };

public:
	void Update(float4x4& cam, Alignment alignment, std::variant<FreeTransform, ComponentTransform*> transf)
	{
		this->alignment = alignment; 

		float3 fwd, right, up;
		float4x4 myMatrix = GetMatrix(transf);

		switch (alignment)
		{
		case FreeBillBoard::Alignment::axis:
			break; 
		case FreeBillBoard::Alignment::screen:
		{
			fwd = cam.WorldZ().Normalized().Neg();
			up = cam.WorldY().Normalized();
			right = up.Cross(fwd).Normalized(); 

			break;
		}
		
		case FreeBillBoard::Alignment::world:
		{
			fwd = float3(cam.TranslatePart() - myMatrix.TranslatePart()).Normalized();
			right = fwd.Normalized().Cross(cam.WorldY()).Normalized(); // cam.WorldY().Normalized().Cross(fwd).Normalized();
			up = fwd.Cross(right).Normalized();

			break;
		}
		
		default:
			break;
		}

		Quat rot = Quat(float3x3(right, up, fwd)); 
		if (transf.index() == 1)
			std::get<ComponentTransform*>(transf)->ChangeRotation(rot.Inverted());
			
		else
			std::get<FreeTransform>(transf).UpdateGlobalMatrix(myMatrix);
	}; 

private:
	Alignment alignment;
	float4x4 GetMatrix(std::variant<FreeTransform, ComponentTransform*> transf) const { return (transf.index() == 0) ? std::get<FreeTransform>(transf).GetGlobalMatrix() : std::get<ComponentTransform*>(transf)->GetGlobalMatrix(); };

	friend class GameObject; 
};
