#include "ComponentVolatile.h"
#include "SmileGameObjectManager.h"
#include "SmileApp.h"
#include "ComponentTransform.h"

ComponentVolatile::ComponentVolatile(float sTime, void(*destroyFunc)()) : sTime(sTime), destroyFunc(destroyFunc)
{
	type = COMPONENT_TYPE::VOLATILE;
	SetName("Volatile");
};

ComponentVolatile::ComponentVolatile(float sTime) : sTime(sTime)
{
	type = COMPONENT_TYPE::VOLATILE;
	SetName("Volatile");
};


ComponentVolatile::ComponentVolatile(float sTime, void(*destroyFunc)(), float3 speed) : sTime(sTime), destroyFunc(destroyFunc), speed(speed)
{
	type = COMPONENT_TYPE::VOLATILE;
	SetName("Volatile");
};


void ComponentVolatile::Update(float dt)
{
	if ((currentTime += dt) >= sTime)
	{
		App->object_manager->toDestroy.push_back(GetParent()); 
		if(destroyFunc)
			destroyFunc();
		return; 
	}

	if (speed.IsFinite())
	{
		auto t = GetParent()->GetTransform(); 
		auto mat = t->GetGlobalMatrix(); 
		mat.SetTranslatePart(mat.TranslatePart() + speed * dt);
		t->SetGlobalMatrix(mat);
	}
}