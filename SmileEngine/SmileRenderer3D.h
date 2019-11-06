#pragma once
#include "SmileModule.h"
#include "SmileSetup.h"
#include "glmath.h"
#include "Light.h"

#define MAX_LIGHTS 8

struct renderingData
{
	// Planes
	float2 pNearSize = float2(); 
	float2 pFarSize = float2();

	// Fov
	float fovYangle = 60.f; 
	float pNearDist = 1.f, pFarDist = 512.f; 
	float ratio = pNearDist / pFarDist; 
};

class SmileRenderer3D : public SmileModule
{
public:
	SmileRenderer3D(SmileApp* app, bool start_enabled = true);
	~SmileRenderer3D();

	bool Init();
	update_status PreUpdate(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	void OnResize(int width, int height);
	renderingData GetData() const { return _renderingData; };
private: 
	void ComputeSpatialData();
	renderingData _renderingData;
public:
	SDL_GLContext context;
	Light lights[MAX_LIGHTS];
	mat3x3 NormalMatrix;
	mat4x4 ModelMatrix, ViewMatrix, ProjectionMatrix;
	
};