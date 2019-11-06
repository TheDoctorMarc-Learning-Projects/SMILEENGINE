#pragma once

#include "glmath.h"
#include "MathGeoLib/include/MathGeoLib.h"


#define MIN_DIST_TO_MESH 5.F
#define MAX_FRAME_SPEED 10.F
#define EXPONENTIAL_ZOOM_FACTOR 1.5F
#define DEFAULT_SPEED 30.F

#include <array>
class Frustrum
{
public: 
	Frustrum(GameObjectCamera* camera);
	~Frustrum() {};

	enum INTERSECTION_TYPE
	{
		NONE,
		PARTIAL,
		ALL
	};

	struct plane
	{
		float3 vertices[4]; 
	//	INTERSECTION_TYPE GetIntersection(float3 vertex);
	};

public: 
	std::array<plane, 6> GetPlanes() const { return planes; }; 
	//void DebugPlanes(); 

private: 
	void CalculatePlanes(); 
	std::array<plane, 6> planes;
	GameObjectCamera* myCamera = nullptr; 
}; 



class GameObject; 
class GameObjectCamera : public GameObject
{
public:
	GameObjectCamera(GameObject* parent);
	GameObjectCamera(GameObject* parent, vec3 Position, vec3 Reference);
	~GameObjectCamera();

	void Update(); 
	
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
	vec3 X, Y, Z, Reference;

private:
	mat4x4 ViewMatrix, ViewMatrixInverse;
	Frustrum* frustrum = nullptr; 
};