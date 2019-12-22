#pragma once

#include "MathGeoLib/include/Math/float4x4.h"
#include "MathGeoLib/include/Math/float3.h"

#include <variant>

#include "FreeBillBoard.h"
#include "ComponentTransform.h"
#include "FreeTransform.h"

class FreeBillBoard
{

public:
	enum Alignment { axis, screen, world, noAlignment };

public:
	FreeBillBoard(Alignment alignment, float4x4& cam, std::variant<FreeTransform*, ComponentTransform*> transf) : alignment(alignment), transf(transf) { Update(cam); };
	FreeBillBoard() {};
	~FreeBillBoard() {};

public:
	void Update(float4x4& cam)
	{
		float3 fwd, right, up;
		float4x4 myMatrix = GetMatrix(); 
		switch (alignment)
		{
		case FreeBillBoard::Alignment::axis:
			break; 
		case FreeBillBoard::Alignment::screen:
			break;
		case FreeBillBoard::Alignment::world:
		{
			fwd = float3(cam.TranslatePart() - myMatrix.TranslatePart()).Normalized();
			right = cam.WorldY().Normalized().Cross(fwd).Normalized();
			up = fwd.Cross(right).Normalized();
		}
			break;
		default:
			break;
		}

		float4x4 res(right.ToDir4(), up.ToDir4(), fwd.ToDir4(), GetMatrix().TranslatePart().ToPos4());
		if (transf.index() == 1)
			std::get<ComponentTransform*>(transf)->SetGlobalMatrix(res);
		else
			std::get<FreeTransform*>(transf)->UpdateGlobalMatrix(res);
	}; 

private:
	float4x4 GetMatrix()const { return (transf.index() == 0) ? std::get<FreeTransform*>(transf)->GetGlobalMatrix() : std::get<ComponentTransform*>(transf)->GetGlobalMatrix(); };

private:
	Alignment alignment = Alignment::noAlignment;
	std::variant<FreeTransform*, ComponentTransform*> transf;
};
