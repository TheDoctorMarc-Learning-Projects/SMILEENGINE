#pragma once

#include "MathGeoLib/include/Math/float3.h"
#include "MathGeoLib/include/Math/float2.h"

#define MIN_DIST_TO_MESH 5.F
#define MAX_FRAME_SPEED 10.F
#define EXPONENTIAL_ZOOM_FACTOR 1.5F
#define DEFAULT_SPEED 30.F

#include <array>
// ----------------------------------------------------------------- [Frustrum]
class Frustrum
{
public: 
	Frustrum(GameObjectCamera* camera);
	~Frustrum() {};

	enum class INTERSECTION_TYPE
	{
		OUTSIDE,
		INTERSECT,
		INSIDE
	};

	struct plane
	{
		float3 vertices[4]; 
		float3 normal = float3(0, 0, 0); 
		float3 center = float3(0, 0, 0);
		INTERSECTION_TYPE GetIntersection(float3 vertex);
	};

public: 
	std::array<plane, 6> GetPlanes() const { return planes; }; 
	void DebugPlanes(); 
	INTERSECTION_TYPE IsCubeInsideFrustrumView(AA_BB& box);
private: 
	void CalculatePlanes(); 
	std::array<plane, 6> planes;
	GameObjectCamera* myCamera = nullptr; 
}; 

// ----------------------------------------------------------------- [Render preferences]
struct renderingData
{
public: 
	// Fov
	float fovYangle = 60.f;
	float pNearDist = 1.f, pFarDist = 512.f;
	float ratio = InitRatio();

	// Use this to compute the radio!
	float InitRatio();

private: 
	// Planes
	float2 pNearSize = float2();
	float2 pFarSize = float2();


	friend class GameObjectCamera; 
	friend class Frustrum; 
};

#include "glmath.h"
// ----------------------------------------------------------------- [Camera]
class GameObject; 
class GameObjectCamera : public GameObject
{
public:
	GameObjectCamera(GameObject* parent, renderingData data = {});
	GameObjectCamera(GameObject* parent, vec3 Position, vec3 Reference, renderingData data = {});
	~GameObjectCamera();

	void Update(); 
	
	// Do stuff 
	void Look(const vec3& Position, const vec3& Reference, bool RotateAroundReference = false);
	void LookAt(const vec3& Spot); // oh yes indeed
	void LookAt(const float3& Spot); // oh yes indeed
	void Move(const vec3& Movement);
	void FitCameraToObject(GameObject* obj);

	// Getters
	float* GetViewMatrix();
	float* GetViewMatrixInverse();
	float GetScrollSpeed(float dt, float zScroll);
	renderingData GetRenderingData() const { return _renderingData; };
	Frustrum* GetFrustrum() const { return frustrum; }; 
private:
	void CalculateViewMatrix();
	void FocusObjectLogic();
	void ComputeSpatialData(); 

public:
	vec3 X, Y, Z, Reference;

private:
	mat4x4 ViewMatrix, ViewMatrixInverse;
	Frustrum* frustrum = nullptr; 
	renderingData _renderingData;
};