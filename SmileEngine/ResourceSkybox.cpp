#include "ResourceSkybox.h"
#include "Resource.h"
#include "SmileApp.h"
#include "SmileResourceManager.h"
#include "ResourceMeshPlane.h"
#include "ResourceTexture.h"
#include "RNG.h"
#include "MathGeoLib/include/Math/Quat.h"

ResourceSkybox::ResourceSkybox(SmileUUID uuid, Resource_Type type, std::string texPaths[6], float radius) :
	Resource(uuid, type, "multiPath"), radius(radius)
{

	for (int i = 0; i < 6; ++i)
	{
		auto mesh = DBG_NEW ResourceMeshPlane(RNG::GetRandomUUID(), ownMeshType::plane, "Default", float4::inf,
			nullptr, radius);
		App->resources->resources.insert(std::pair<SmileUUID, Resource*>(mesh->GetUID(), (Resource*)mesh));
		App->resources->UpdateResourceReferenceCount(mesh->GetUID(), 1);
		 
		auto texture = (ResourceTexture*)App->resources->GetResourceByPath(texPaths[i].c_str());
		if (texture == nullptr)
		{
			texture = (ResourceTexture*)App->resources->CreateNewResource(RESOURCE_TEXTURE, texPaths[i]);
			texture->LoadOnMemory(texPaths[i].c_str());
		}

		planes[i] = mesh; 
		textures[i] = texture; 
	    
	}
 
}

void ResourceSkybox::Draw()
{
	static bool tile = false; 
	for (int i = 0; i < 6; ++i)
		planes[i]->BlitMeshHere(GetPlaneTransform(i), tile, textures[i]);
}
			



float4x4 ResourceSkybox::GetPlaneTransform(uint index) // left back right front top down
{
	float4x4 ret = float4x4::identity; 
	
	switch (index)
	{
	case 0:
	{
		ret.SetRotatePart(float3(0, 1, 0), 90 * DEGTORAD); 
		ret.SetTranslatePart(float3(-radius / 2.f, 0, 0)); 
		break; 
	}
	case 1:
	{
		ret.SetTranslatePart(float3(0, 0, -radius / 2.f));
		break;
	}
	case 2:
	{
		ret.SetRotatePart(float3(0, 1, 0), -90 * DEGTORAD);
		ret.SetTranslatePart(float3(radius / 2.f, 0, 0));
		break;
	}
	case 3:
	{
		ret.SetRotatePart(float3(0, 1, 0), 180 * DEGTORAD);
		ret.SetTranslatePart(float3(0, 0, radius / 2.f));
		break;
	}
	case 4:
	{
		ret.SetRotatePart(float3(1, 0, 0), 90 * DEGTORAD);
		ret.SetTranslatePart(float3(0, radius / 2.f, 0));
		break;
	}
	case 5:
	{	   
		ret.SetRotatePart(float3(1, 0, 0), -90 * DEGTORAD);
		ret.SetTranslatePart(float3(0, -radius / 2.f, 0));
		break;
	}
	default:
		break;
	}

	return ret; 
}