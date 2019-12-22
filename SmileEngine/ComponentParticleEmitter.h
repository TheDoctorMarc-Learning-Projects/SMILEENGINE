#pragma once

#include <vector>
#include <map>
#include <variant>
#include <any>

#include "MathGeoLib/include/Math/float4.h"


class FreeTransform;
class FreeBillBoard;

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
};

struct Particle
{
	/*FreeTransform transf;
	FreeBillBoard billboard;*/
	CurrentState currentState;
};

// Once the particle system is created, this initialState is set to the recieved values 
// 1) Initial Value 2) Value over time, or no value (false)

struct InitialState
{
	// Particle Variables
	bool active = false;

	// This Variables will be updated each frame if they have value over time (Current order: 0->5)
	std::pair<float, std::variant<float, bool>> life = std::pair(100.f, 1.f);
	std::pair<float3, std::variant<float3, bool>> speed = std::pair(float3::one, float3::one);
	std::pair<float, std::variant<float, bool>> size = std::pair(1.f, 0.f);
	std::pair<float, std::variant<float, bool>> alpha = std::pair(1.f, 0.f);
	std::pair<float4, std::variant<float4, bool>> color = std::pair(float4::zero, float4::zero);
	std::pair<float, std::variant<float, bool>> tex = std::pair(0.f, 0.f);

};

struct emissionData
{
	uint_fast8_t maxParticles = 100;
	bool loop = true;
	float rate = 1.f; 
};

enum emmissionShape { CIRCLE, CUBE, CONE };
struct emmssionMode { emmissionShape shape; float angle, radius; };

class ComponentParticleEmitter; 
typedef void (ComponentParticleEmitter::*function)(Particle& p, float dt);

class ComponentParticleEmitter
{
public: 
	ComponentParticleEmitter(InitialState initialState);
	~ComponentParticleEmitter();
public: 
	enum blendMode{ ADDITIVE, ALPHA_BLEND };
	enum lightMode { PER_EMITTER, PER_PARTICLE, NONE}; // TODO: how to handle this?
	emmssionMode emmissionMode;
	emissionData emissionData; 

public: 
	void Update(float dt); 

private: 
	void Draw(); 
	inline void SpeedUpdate(Particle& p, float dt);
	inline void LifeUpdate(Particle& p, float dt);

private: 
	std::vector<Particle> particles; 
	InitialState initialState;
	std::map<uint_fast8_t, function> pVariableFunctions; // They co-relate by order to particle state variables (Current order: 0->5)

};
