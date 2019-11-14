#pragma once
#include "SmileModule.h"
#include "SmileSetup.h"
#include "glmath.h"
#include "Light.h"

#define MAX_LIGHTS 8

class SmileRenderer3D : public SmileModule
{
public:
	SmileRenderer3D(SmileApp* app, bool start_enabled = true);
	~SmileRenderer3D();

	bool Init();
	bool Start(); 
	update_status PreUpdate(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	void OnResize(int width, int height);
	float* GetProjectionMatrix(); 

public:
	SDL_GLContext context;
	Light lights[MAX_LIGHTS];
	mat3x3 NormalMatrix;
	mat4x4 ModelMatrix, ViewMatrix, ProjectionMatrix;
	ComponentCamera* targetCamera = nullptr; 
	
};