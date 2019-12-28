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
	enum Alignment { axis, screen, world, noAlignment }; 

public:
	void Update(float4x4& cam, Alignment alignment, ComponentTransform* transf, FreeTransform& fTransf = null)
	{
		this->alignment = alignment; 

		float3 fwd, right, up;
		float4x4 myMatrix = (transf) ? transf->GetGlobalMatrix() : fTransf.GetGlobalMatrix(); 
		Quat rot; 
		auto camComp = App->scene_intro->gameCamera; 
		switch (alignment)
		{
		case FreeBillBoard::Alignment::axis:
			break; 
		case FreeBillBoard::Alignment::screen:
		{
			float4x4 viewMatrix = cam;
			float3x3 aRot = float3x3(viewMatrix.WorldX(), viewMatrix.WorldY(), viewMatrix.WorldZ());
			rot = aRot.ToQuat();

			break;
		}
		
		case FreeBillBoard::Alignment::world:
		{
			/*fwd = (camComp->GetParent()->GetTransform()->GetGlobalPosition() - myMatrix.TranslatePart()).Normalized();
			up = cam.WorldY();
			right = up.Cross(fwd);
			up = fwd.Cross(right);

			rot = float3x3(right, up, fwd).ToQuat();*/

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
	 

	friend class GameObject; 
};
