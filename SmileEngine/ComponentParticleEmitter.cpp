#include "ComponentParticleEmitter.h"

ComponentParticleEmitter::ComponentParticleEmitter(InitialState initialState) : initialState(initialState)
{
	// Only push a function if the variable changes on time
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
  
}

// -----------------------------------------------------------------
ComponentParticleEmitter::~ComponentParticleEmitter()
{
	pVariableFunctions.clear(); 
}

// -----------------------------------------------------------------
void ComponentParticleEmitter::Update(float dt)
{
	// Loop particles. Quickly discard inactive ones. Execute only needed functions 
	for (int i = 0; i < particles.size() && particles.at(i).currentState.active; ++i)
		for (auto func = pVariableFunctions.begin(); func != pVariableFunctions.end(); ++func)
				(this->*(func->second))(particles.at(i), dt);
			

	
}

// -----------------------------------------------------------------
void ComponentParticleEmitter::Draw()
{
	// TODO -> sort alive particles from far to near
}

// ----------------------------------------------------------------- Update Values
inline void ComponentParticleEmitter::LifeUpdate(Particle& p, float dt)
{
	// This is placeholder 

	if ((p.currentState.life -= std::get<float>(initialState.life.second)* dt) <= 0.f)
	{
		p.currentState.active = false; 
	}
}

// -----------------------------------------------------------------
inline void ComponentParticleEmitter::SpeedUpdate(Particle& p, float dt)
{
	// get particle free transform and add the speed to the position (locally updated!)
 
}

