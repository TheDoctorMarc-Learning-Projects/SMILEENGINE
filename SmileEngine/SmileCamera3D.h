#pragma once
#include "SmileModule.h"
#include "SmileSetup.h"
#include "glmath.h"
#include "ComponentMesh.h"
#include "MathGeoLib/include/MathGeoLib.h"

#define MIN_DIST_TO_MESH 5.F
#define MAX_FRAME_SPEED 10.F
#define EXPONENTIAL_ZOOM_FACTOR 1.5F
#define DEFAULT_SPEED 30.F

class SmileCamera3D : public SmileModule
{
public:
	SmileCamera3D(SmileApp* app, bool start_enabled = true);
	~SmileCamera3D();

	bool Start();
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	bool CleanUp();

	void Look(const vec3& Position, const vec3& Reference, bool RotateAroundReference = false);
	void LookAt(const vec3& Spot); // oh yes indeed
	void LookAt(const float3& Spot); // oh yes indeed
	void Move(const vec3& Movement);
	float* GetViewMatrix();
	float* GetViewMatrixInverse();

	void FitCameraToObject(GameObject* obj);

	float GetScrollSpeed(float dt, float zScroll); 

private:

	void CalculateViewMatrix();
	void FocusObjectLogic(); 

public:

	vec3 X, Y, Z, Position, Reference;

private:

	mat4x4 ViewMatrix, ViewMatrixInverse;
};