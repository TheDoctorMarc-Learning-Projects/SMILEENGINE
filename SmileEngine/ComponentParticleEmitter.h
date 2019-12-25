#pragma once

#include <vector>
#include <map>
#include <variant>
#include <any>

#include "MathGeoLib/include/Math/float4.h"

#include "Component.h"


#include "FreeBillBoard.h"
#include "FreeTransform.h"

struct InitialRandomState
{
	float3 speed = float3::inf; 
	float4 color = float4::inf; 
};

struct CurrentState
{
	bool active;
	float life;
	float3 speed;
	float size;
	float transparency;
	float4 color;
	uint tileIndex = 0;  
	float lastTileframe = 0.f; 
	// Stuff that is random, per-particle, can be stored here for updation:
	InitialRandomState randomData;
};

struct Particle
{
	FreeTransform transf;
	FreeBillBoard billboard;
	CurrentState currentState;

	float camDist = -floatMax; 
	bool operator<(Particle& other) { return this->camDist > other.camDist; }
};

// Once the particle system is created, this initialState is set to the recieved values 
// 1) Initial Value 2) Value over time, or no value (false)

struct InitialState
{
	// This Variables will be updated each frame if they have value over time (Current order: 0->5)
	std::pair<float, float> life = std::pair(1.f, 1.f);
	std::pair<float3, float3> speed = std::pair(float3::zero, float3::zero); // initial & over time
	std::pair<float, float> size = std::pair(1.f, 0.f); // initial & final
	std::pair<float, float> transparency = std::pair(0.f, 0.f);
	std::pair<float4, float4> color = std::pair(float4::inf, float4::inf); // initial & final
	std::pair<bool, float> tex = std::pair(false, 0.f); // has & anim speed 

};

enum class emmissionShape { CIRCLE, CUBE, CONE }; // ... 
enum class blendMode { ADDITIVE, ALPHA_BLEND };
enum class lightMode { PER_EMITTER, PER_PARTICLE, NONE };


struct EmissionData
{
	uint_fast8_t maxParticles = 100;
	bool loop = true;
	std::string texPath = "empty"; 
	std::pair<bool, std::variant<float3, std::pair<float3, float3>>> randomSpeed;
	std::pair<bool, std::variant<float4, std::pair<float4, float4>>> randomColor; 
	float time = 0.5f, currenTime = 0.f, angle = 0.f, radius = 1.f;
	emmissionShape shape = emmissionShape::CONE;
};

// This struct has it all: 
struct AllData
{
	// Generation
	EmissionData emissionData; 

	// Initial State
	InitialState initialState;

	// Modes
	blendMode blendmode = blendMode::ALPHA_BLEND;
	lightMode lightmode = lightMode::NONE; // TODO: how to handle this? light settings should be supported in a struct
};


class ComponentParticleEmitter; 
typedef void (ComponentParticleEmitter::*function)(Particle& p, float dt);

class ResourceMeshPlane;
class ResourceTexture;
class GameObject; 
class ComponentTransform; 
class ComponentParticleEmitter: public Component
{
public: 
	ComponentParticleEmitter(GameObject* parent); // Default emitter
	ComponentParticleEmitter(GameObject* parent, AllData data); // User defined 
	~ComponentParticleEmitter();

public: 
	void Update(float dt = 0); 
	void CleanUp(); 
	void OnSave();

private: 
	void SetupMesh(); 
	void Draw();  
	void SpawnParticle(); 
	float3 GetRandomRange(std::variant<float3, std::pair<float3, float3>> ranges);
	float4 GetRandomRange4(std::variant<float4, std::pair<float4, float4>> ranges);
	inline void SpeedUpdate(Particle& p, float dt);
	inline void LifeUpdate(Particle& p, float dt);
	inline void ColorUpdate(Particle& p, float dt);
	inline void AnimUpdate(Particle& p, float dt); 
	
private: 
	uint_fast8_t lastUsedParticle = 0;
	std::vector<Particle> particles, drawParticles; 
	AllData data;
	std::map<uint_fast8_t, function> pVariableFunctions; // They co-relate by order to particle state variables (Current order: 0->5)
	
public: 
	ResourceMeshPlane* mesh = nullptr; 
	ResourceTexture* texture = nullptr; 

	// a pointer for easier access: 
	float4x4 camMatrix; 
};



