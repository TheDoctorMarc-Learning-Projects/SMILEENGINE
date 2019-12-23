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
}

ComponentParticleEmitter::ComponentParticleEmitter(GameObject* parent, EmissionData emissionData) : emissionData(emissionData)
{
	type = COMPONENT_TYPE::EMITTER;
	SetName("Emitter");

	// Only push a function if the variable changes on time
	/*auto initialState = emissionData.initialState; 

	if (initialState.life.second.index() == 0)
		pVariableFunctions.insert(std::pair((uint_fast8_t)0, &ComponentParticleEmitter::LifeUpdate)); 
	if (initialState.speed.second.index() == 0)
		pVariableFunctions.insert(std::pair((uint_fast8_t)1, &ComponentParticleEmitter::SpeedUpdate));
/*	if (initialState.size.second.index() == 0)
	//	pVariableFunctions.insert(std::pair((uint_fast8_t)2, &ComponentParticleEmitter::LifeUpdate));
	if (initialState.alpha.second.index() == 0)
	//	pVariableFunctions.insert(std::pair((uint_fast8_t)3, &ComponentParticleEmitter::LifeUpdate));
	if (initialState.color.second.index() == 0)
	//	pVariableFunctions.insert(std::pair((uint_fast8_t)4, &ComponentParticleEmitter::LifeUpdate));
	if (initialState.tex.second.index() == 0)
	//	pVariableFunctions.insert(std::pair((uint_fast8_t)5, &ComponentParticleEmitter::LifeUpdate));*/
  

	// Fill the particles buffer -> TODO: fill with initial state captured from this constructor
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
	for (int i = 0; i < particles.size() && particles.at(i).currentState.life > 0.f; ++i)
		for (auto func = pVariableFunctions.begin(); func != pVariableFunctions.end(); ++func)
				(this->*(func->second))(particles.at(i), dt);
			
	// Spawn new particles.
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
	for (int i = lastUsedParticle; i < particles.size(); i++) {
		if (particles[i].currentState.life <= 0.f) {
			lastUsedParticle = i;
			return i;
		}
	}

	for (int i = 0; i < lastUsedParticle; i++) {
		if (particles[i].currentState.life <= 0.f) {
			lastUsedParticle = i;
			return i;
		}
	}

	return 0; 
}

// -----------------------------------------------------------------
void ComponentParticleEmitter::SpawnParticle()
{
	// Reset Emission Time
	emissionData.currenTime = 0.f; 

	// Find Available Particle
	Particle& p = particles[FindAvailableParticleIndex(particles, lastUsedParticle)]; 

	// Set Particle State
	p.currentState.alpha = emissionData.initialState.alpha.first; 
	p.currentState.color = emissionData.initialState.color.first;
	p.currentState.life = emissionData.initialState.life.first; 
	p.currentState.size = emissionData.initialState.size.first; 
	p.currentState.speed = emissionData.initialState.speed.first; 
	p.currentState.tex = emissionData.initialState.tex.first; 

	// Set particle Transform
	p.transf.parentMatrix = GetParent()->GetTransform()->GetLocalMatrix(); 
	p.transf.UpdateGlobalMatrix(math::float4x4::FromTRS(GetSpawnPos(), float4x4::identity, float3::one)); 

	// Particle billboard will be updated in Update (XD)
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
    
}

