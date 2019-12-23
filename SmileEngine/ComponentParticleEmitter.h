#pragma once

#include <vector>
#include <map>
#include <variant>
#include <any>

#include "MathGeoLib/include/Math/float4.h"

#include "Component.h"


#include "FreeBillBoard.h"
#include "FreeTransform.h"

struct randomData
{
	float3 speed = float3::inf; 
};
struct CurrentState
{
	bool active;
	float life;
	float3 speed;
	float size;
	float alpha;
	float4 color;
	// anim frame ? 
	float tex;

	// Stuff that is random (per-particle= can be stored here for updation:
	randomData randomData;
};

struct Particle
{
	FreeTransform transf;
	FreeBillBoard billboard;
	CurrentState currentState;
};

// Once the particle system is created, this initialState is set to the recieved values 
// 1) Initial Value 2) Value over time, or no value (false)

struct InitialState
{
	// This Variables will be updated each frame if they have value over time (Current order: 0->5)
	std::pair<float, float> life = std::pair(1.f, 1.f);
	std::pair<float3, float3> speed = std::pair(float3::zero, float3::zero); 
	std::pair<float, float> size = std::pair(1.f, 0.f);
	std::pair<float, float> alpha = std::pair(1.f, 0.f);
	std::pair<float4, float4> color = std::pair(float4::zero, float4::zero);
	std::pair<float, float> tex = std::pair(0.f, 0.f);

};

enum class emmissionShape { CIRCLE, CUBE, CONE }; // ... 
enum class blendMode { ADDITIVE, ALPHA_BLEND };
enum class lightMode { PER_EMITTER, PER_PARTICLE, NONE };

// This struct has it all: 
struct EmissionData
{
	// Generation
	uint_fast8_t maxParticles = 100;
	bool loop = true;
	std::pair<bool, std::variant<float3, std::pair<float3, float3>>> randomSpeed; 
	float time = 0.5f, currenTime = 0.f, angle = 0.f, radius = 1.f;
	emmissionShape shape = emmissionShape::CONE;

	// Initial State
	InitialState initialState;

	// Modes
	blendMode blendmode = blendMode::ADDITIVE; 
	lightMode lightmode = lightMode::NONE; // TODO: how to handle this? light settings should be supported in a struct
};


class ComponentParticleEmitter; 
typedef void (ComponentParticleEmitter::*function)(Particle& p, float dt);

class ResourceMesh; 
class ResourceMaterial; 
class GameObject; 
class ComponentParticleEmitter: public Component
{
public: 
	ComponentParticleEmitter(GameObject* parent); // Default emitter
	ComponentParticleEmitter(GameObject* parent, EmissionData emissionData); // User defined 
	~ComponentParticleEmitter();

public: 
	void Update(float dt = 0); 
	void CleanUp(); 

private: 
	void Draw(); 
	void SpawnParticle(); 
	float3 GetSpawnPos(); 
	float3 GetRandomRange(float3& toModify, std::variant<float3, std::pair<float3, float3>> ranges);
	inline void SpeedUpdate(Particle& p, float dt);
	inline void LifeUpdate(Particle& p, float dt);

private: 
	uint_fast8_t lastUsedParticle = 0;
	std::vector<Particle> particles; // how to fill this? what about the size?
	EmissionData emissionData;
	std::map<uint_fast8_t, function> pVariableFunctions; // They co-relate by order to particle state variables (Current order: 0->5)
	ResourceMesh* mesh = nullptr; 
	ResourceMaterial* mat = nullptr;  
};



