#include "ComponentParticleEmitter.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"
#include "SmileApp.h"
#include "SmileResourceManager.h"
#include "ResourceMeshPlane.h"
#include "ResourceMesh.h"
#include "FreeBillBoard.h"
#include "FreeTransform.h"
#include "GameObject.h"
#include "RNG.h"

// TODO: copy the initial values! Maybe have an instance of "initialValues" predefined too for the default ctor 

ComponentParticleEmitter::ComponentParticleEmitter(GameObject* parent)
{
	type = COMPONENT_TYPE::EMITTER;
	SetName("Emitter"); 
	particles.resize(emissionData.maxParticles);

	pVariableFunctions.insert(std::pair((uint_fast8_t)0, &ComponentParticleEmitter::LifeUpdate));
	pVariableFunctions.insert(std::pair((uint_fast8_t)1, &ComponentParticleEmitter::SpeedUpdate));

	// Store Initial Variables Only if not random (eg speed) 
}

ComponentParticleEmitter::ComponentParticleEmitter(GameObject* parent, EmissionData emissionData) : emissionData(emissionData)
{
	type = COMPONENT_TYPE::EMITTER;
	SetName("Emitter");

	// Only push a function if the variable changes on time
	auto initialState = emissionData.initialState; 

	if (initialState.life.second > 0.f)
		pVariableFunctions.insert(std::pair((uint_fast8_t)0, &ComponentParticleEmitter::LifeUpdate)); 
	if (initialState.speed.second.Length() > 0.f || emissionData.randomSpeed.first)
		pVariableFunctions.insert(std::pair((uint_fast8_t)1, &ComponentParticleEmitter::SpeedUpdate));


	// Fill the particles buffer  
	particles.resize(emissionData.maxParticles);

}

// -----------------------------------------------------------------
ComponentParticleEmitter::~ComponentParticleEmitter()
{
	/*pVariableFunctions.clear(); 

	for (auto& p : particles)
	{
		RELEASE(p.billboard); 
		RELEASE(p.transf); 
	}
	particles.clear(); */
}

void ComponentParticleEmitter::CleanUp()  
{
	pVariableFunctions.clear();
	particles.clear();
}

// -----------------------------------------------------------------
void ComponentParticleEmitter::Update(float dt)
{
	// Loop particles. Quickly discard inactive ones. Execute only needed functions 
	for (int i = 0; i < particles.size(); ++i)
		if(particles.at(i).currentState.life > 0.f)
			for (auto func = pVariableFunctions.begin(); func != pVariableFunctions.end(); ++func)
				(this->*(func->second))(particles.at(i), dt);
		
	// Spawn new particles
	if ((emissionData.currenTime += dt) > emissionData.time)
		SpawnParticle(); 
	
	// Finally Draw
	Draw(); 
}

// -----------------------------------------------------------------
void ComponentParticleEmitter::Draw()
{
	// TODO -> sort alive particles from far to near (not active ones last)

	// Blit them 
	for (auto& p : particles)
		if (p.currentState.life > 0.f)
			App->resources->Plane->BlitMeshHere(p.transf.GetGlobalMatrix(),
			(emissionData.initialState.tex.first > 0.f) ? GetParent()->GetMaterial()->GetResourceTexture(): nullptr,
				emissionData.blendmode, p.currentState.alpha); 
}

// -----------------------------------------------------------------
inline static int FindAvailableParticleIndex(std::vector<Particle>& particles, uint_fast8_t& lastUsedParticle)
{
	for (int i = lastUsedParticle; i < particles.size(); i++) 
	{
		if (particles[i].currentState.life <= 0.f)
		{
			lastUsedParticle = i;
			return i;
		}
	}

	for (int i = 0; i < lastUsedParticle; i++)
	{
		if (particles[i].currentState.life <= 0.f) 
		{
			lastUsedParticle = i;
			return i;
		}
	}

	return 0; 
}

// -----------------------------------------------------------------
void ComponentParticleEmitter::SpawnParticle()
{
	// 1) Reset Emission Time
	emissionData.currenTime = 0.f; 

	// 2) Find Available Particle
	Particle& p = particles[FindAvailableParticleIndex(particles, lastUsedParticle)]; 

	// 3) Set Particle State
	p.currentState.alpha = emissionData.initialState.alpha.first; 
	p.currentState.color = emissionData.initialState.color.first;
	p.currentState.life = emissionData.initialState.life.first; 
	p.currentState.size = emissionData.initialState.size.first; 
	p.currentState.tex = emissionData.initialState.tex.first; 

	// speed is kinda interesting:
	bool random = emissionData.randomSpeed.first; 
	p.currentState.speed = (random) ? (GetRandomRange(p.currentState.randomData.speed, emissionData.randomSpeed.second)) : emissionData.initialState.speed.second;
		 

	// 4) Set particle Transform
	p.transf.parentMatrix = GetParent()->GetTransform()->GetLocalMatrix(); 
	p.transf.UpdateGlobalMatrix(math::float4x4::FromTRS(GetSpawnPos(), float4x4::identity, float3::one)); 

	// Particle billboard will be updated in Update (XD)
}

// -----------------------------------------------------------------
float3 ComponentParticleEmitter::GetRandomRange(float3& toModify, std::variant<float3, std::pair<float3, float3>> ranges)
{
	 
	if (ranges.index() == 0)
	{
		auto range = std::get<float3>(ranges); 
		toModify.x = std::get<float>(RNG::GetRandomValue(-range.x / 2, range.x / 2));
		toModify.y = std::get<float>(RNG::GetRandomValue(-range.y / 2, range.y / 2));
		toModify.z = std::get<float>(RNG::GetRandomValue(-range.z / 2, range.z / 2));
	}
	else
	{
		auto range = std::get<std::pair<float3, float3>>(ranges);
		toModify.x = std::get<float>(RNG::GetRandomValue(range.first.x, range.second.x));
		toModify.y = std::get<float>(RNG::GetRandomValue(range.first.y, range.second.y));
		toModify.z = std::get<float>(RNG::GetRandomValue(range.first.z, range.second.z));
	}

	return toModify;
}


// -----------------------------------------------------------------
float3 ComponentParticleEmitter::GetSpawnPos()
{
	// TODO --> define an area for each spawn shape (except cone)
	float3 ofs = float3(); 
	ofs.x = std::get<float>(RNG::GetRandomValue(-emissionData.radius / 2, emissionData.radius / 2));
	ofs.y = std::get<float>(RNG::GetRandomValue(-emissionData.radius / 2, emissionData.radius / 2));
	ofs.z = std::get<float>(RNG::GetRandomValue(-emissionData.radius / 2, emissionData.radius / 2));

	return float3(GetParent()->GetTransform()->GetGlobalMatrix().TranslatePart() + ofs);
}

// ----------------------------------------------------------------- Update Values
inline void ComponentParticleEmitter::LifeUpdate(Particle& p, float dt)
{
	// This is placeholder 

	if ((p.currentState.life -= emissionData.initialState.life.second * dt) <= 0.f)
		p.currentState.life = 0.f; 
}

// -----------------------------------------------------------------
inline void ComponentParticleEmitter::SpeedUpdate(Particle& p, float dt)
{
	// Add the speed to the particle transform pos. Update the billboard too. Gravity? Yet another variable in the emitter xd
	auto pos = p.transf.globalMatrix.TranslatePart();
	p.transf.globalMatrix.SetTranslatePart(pos += (p.currentState.randomData.speed.IsFinite()) ? p.currentState.randomData.speed : emissionData.initialState.speed.second * dt);
}

