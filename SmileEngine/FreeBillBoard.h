#pragma once

#include "MathGeoLib/include/Math/float4x4.h"
#include "MathGeoLib/include/Math/float3.h"

#include <variant>

#include "FreeBillBoard.h"
#include "ComponentTransform.h"
#include "FreeTransform.h"
#include "SmileApp.h"
#include "SmileScene.h"

struct FreeBillBoard
{

public:
	enum helperAxis { X, Y, Z};
	enum Alignment { axis, screen, world, noAlignment }; 

public:
	void Update(float4x4& cam, Alignment alignment, ComponentTransform* transf, FreeTransform& fTransf = null, helperAxis axis = helperAxis::X)
	{
		this->alignment = alignment; 
		this->helperAxis = helperAxis; 

		float3 fwd, right, up;
		float4x4 myMatrix = (transf) ? transf->GetGlobalMatrix() : fTransf.GetGlobalMatrix(); 
		Quat rot; 
		auto camComp = App->scene_intro->gameCamera; 
		switch (alignment)
		{
		case FreeBillBoard::Alignment::axis:

		{
			float3 dir = fwd = float3(camComp->GetParent()->GetTransform()->GetGlobalPosition() - myMatrix.TranslatePart()).Normalized();
			switch (helperAxis)
			{
			case(X):
				right = float3::unitX;
				up = fwd.Cross(right).Normalized();
				fwd = right.Cross(up).Normalized();
				break;
			case(Y):
				up = float3::unitY;
				right = up.Cross(fwd).Normalized();
				fwd = right.Cross(up).Normalized();
				break;
			case(Z):
				fwd = float3::unitZ;
				right = dir.Cross(fwd).Normalized();
				up = fwd.Cross(right).Normalized();
				break;
			}
			rot = float3x3(right, up, fwd).ToQuat();

			break;

		}
		case FreeBillBoard::Alignment::screen:
		{
			rot = float3x3(cam.WorldX(), cam.WorldY(), cam.WorldZ()).ToQuat();
			break;
		}
		
		case FreeBillBoard::Alignment::world:
		{
			fwd = (camComp->GetParent()->GetTransform()->GetGlobalPosition() - myMatrix.TranslatePart()).Normalized();
			up = cam.WorldY().Normalized();
			right = up.Cross(fwd).Normalized();
			up = fwd.Cross(right).Normalized();
			rot = float3x3(right, up, fwd).ToQuat();

			break;
		}
		
		default:
			break;
		}

		if (transf)
			transf->ChangeRotation(rot);
			
		else
			fTransf.ChangeRotation(rot);
	}; 

private:
	Alignment alignment;
	helperAxis helperAxis = X; 

	friend class GameObject; 
};
