#pragma once

#include "ResourceMesh.h"
#include "MathGeoLib/include/Math/float4x4.h"
#include "MathGeoLib/include/Math/float4.h"
#include "ComponentParticleEmitter.h" // had to this for an enum
#include "SmileSetup.h"

class ResourceTexture;
struct bufferData
{
	uint id_color; 
	uint num_color; 
    float* color; 

};

struct TileData
{
	uint nRows = 0, nCols = 0, maxTiles = 0; 
};

class ResourceMeshPlane : public ResourceMesh
{
public:
	ResourceMeshPlane::ResourceMeshPlane(SmileUUID uuid, ownMeshType type, std::string path, float4 color = float4::inf, TileData* tileData = nullptr);
	ResourceMeshPlane::~ResourceMeshPlane() {};
	void LoadOnMemory(float4 color = float4::inf);  
	void FreeMemory(); // may have a color buffer 

	void GenerateOwnMeshData(); 
	void BlitMeshHere(float4x4& global_transform, bool& needTileUpdate, ResourceTexture* tex = nullptr, blendMode blendMode = blendMode::ALPHA_BLEND, float transparency = 0.f, float4 color = float4::inf, uint tileIndex = INFINITE);

private: 
	void UpdateTileUvs(bool& needTileUpdate, uint tileIndex);

public: 
	TileData* tileData = nullptr;
private: 
	bufferData bufferData; 
	float4 color; 

}; 
