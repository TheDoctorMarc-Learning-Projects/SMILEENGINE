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
#include "ComponentTransform.h"
#include "ResourceTexture.h"

// TODO: copy the initial values! Maybe have an instance of "initialValues" predefined too for the default ctor 

ComponentParticleEmitter::ComponentParticleEmitter(GameObject* parent)
{
	type = COMPONENT_TYPE::EMITTER;
	SetName("Emitter"); 

	SetupMesh();

	particles.resize(data.emissionData.maxParticles);

	pVariableFunctions.insert(std::pair((uint_fast8_t)0, &ComponentParticleEmitter::LifeUpdate));
	pVariableFunctions.insert(std::pair((uint_fast8_t)1, &ComponentParticleEmitter::SpeedUpdate));
}

ComponentParticleEmitter::ComponentParticleEmitter(GameObject* parent, AllData data) : data(data)
{
	type = COMPONENT_TYPE::EMITTER;
	SetName("Emitter");

	// 0) Base Mesh

	// 1) Push functions --> Only if the variable changes on time
	auto initialState = this->data.initialState;

	if (initialState.life.second > 0.f)
		pVariableFunctions.insert(std::pair((uint_fast8_t)0, &ComponentParticleEmitter::LifeUpdate)); 
	if (initialState.speed.second.Length() > 0.f || this->data.emissionData.randomSpeed.first)
		pVariableFunctions.insert(std::pair((uint_fast8_t)1, &ComponentParticleEmitter::SpeedUpdate));
	if (initialState.color.second.Length4() > 0.f || this->data.emissionData.randomColor.first)
		pVariableFunctions.insert(std::pair((uint_fast8_t)2, &ComponentParticleEmitter::ColorUpdate));

	// 2) A) Get resources  // TODO: texture animation :) what about the file format??
	SetupMesh();

	if (this->data.emissionData.texPath != "empty")
	{
		texture = (ResourceTexture*)App->resources->GetResourceByPath(this->data.emissionData.texPath.c_str());
		if (texture == nullptr)
		{
			texture = (ResourceTexture*)App->resources->CreateNewResource(RESOURCE_TEXTURE, this->data.emissionData.texPath.c_str());
			texture->LoadOnMemory(this->data.emissionData.texPath.c_str());
		}

		this->data.initialState.tex.first = true; 
	}

	// 3) Resize the particles buffer   
	particles.resize(this->data.emissionData.maxParticles);

	// 2) B) 
	App->resources->UpdateResourceReferenceCount(mesh->GetUID(), particles.size());
	if(texture)
		App->resources->UpdateResourceReferenceCount(texture->GetUID(), particles.size());

}

void ComponentParticleEmitter::SetupMesh()
{
	 
	mesh = DBG_NEW ResourceMeshPlane(dynamic_cast<RNG*>(App->utilities->GetUtility("RNG"))->GetRandomUUID(), ownMeshType::plane, "Default", float4(1, 0, 0, 0.3f));
	App->resources->resources.insert(std::pair<SmileUUID, Resource*>(mesh->GetUID(), (Resource*)mesh));
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
	App->resources->UpdateResourceReferenceCount(mesh->GetUID(), -particles.size());
	mesh = nullptr;
	if (texture)
	{
		App->resources->UpdateResourceReferenceCount(texture->GetUID(), -particles.size());
		texture = nullptr;
	}

	pVariableFunctions.clear();
	particles.clear();
}

void ComponentParticleEmitter::OnSave()
{
	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

	writer.StartObject();
	writer.Key("Particles");

	const char* output = buffer.GetString();
	std::string dirPath;
	App->fs->SaveUnique(dirPath, output, buffer.GetSize(), PARTICLES_FOLDER, "particle", "json");
}

// -----------------------------------------------------------------
void ComponentParticleEmitter::Update(float dt)
{
	camMatrix = App->scene_intro->gameCamera->GetViewMatrixF(); 

	// Loop particles. Quickly discard inactive ones. Execute only needed functions 
	for (int i = 0; i < particles.size(); ++i)
		if(particles.at(i).currentState.life > 0.f)
			for (auto func = pVariableFunctions.begin(); func != pVariableFunctions.end(); ++func)
				(this->*(func->second))(particles.at(i), dt);
		
	// Spawn new particles
	if ((data.emissionData.currenTime += dt) > data.emissionData.time)
		SpawnParticle(); 
	
	// Finally Draw
	Draw(); 
}

// -----------------------------------------------------------------
void ComponentParticleEmitter::Draw()
{
	// Sort
	drawParticles = particles; 
	std::sort(drawParticles.begin(), drawParticles.end());

	// Blit  
	for (auto& p : drawParticles)
		if (p.currentState.life > 0.f)
			mesh->BlitMeshHere(p.transf.GetGlobalMatrix(),
			(data.initialState.tex.first) ? texture : nullptr,
				data.blendmode, p.currentState.transparency, p.currentState.color); // TODO: color updates plane resource buffer -> so create new plane resource for each system! otherwise all systems will change the same plane and chaning one color will affect other systems

	drawParticles.clear();
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
	data.emissionData.currenTime = 0.f;

	// 2) Find Available Particle
	Particle& p = particles[FindAvailableParticleIndex(particles, lastUsedParticle)]; 

	// 3) Set Particle State
	p.currentState.transparency = data.initialState.transparency.first;
	p.currentState.life = data.initialState.life.first;
	p.currentState.size = data.initialState.size.first;
	p.currentState.tex = data.initialState.tex.first;

	
	// Initial speed and color can be random:
	bool randomC = data.emissionData.randomColor.first;
	p.currentState.color = (randomC) ? (p.currentState.randomData.color = GetRandomRange4(data.emissionData.randomColor.second)) : data.initialState.color.first;
	bool randomS = data.emissionData.randomSpeed.first; 
	p.currentState.speed = (randomS) ? (p.currentState.randomData.speed = GetRandomRange(data.emissionData.randomSpeed.second)) : data.initialState.speed.second;
		 

	// 4) Set particle Transform
	p.transf.parentMatrix = GetParent()->GetTransform()->GetGlobalMatrix(); 
	auto initial_pos = p.transf.parentMatrix.TranslatePart(); 
	initial_pos += GetRandomRange(float3::FromScalar(data.emissionData.radius)); 
	p.transf.UpdateGlobalMatrix(math::float4x4::FromTRS(initial_pos, float4x4::identity, float3::one));

	// Particle billboard will be updated in Update (XD)
}

// ----------------------------------------------------------------- Update Values
inline void ComponentParticleEmitter::LifeUpdate(Particle& p, float dt)
{
	// This is placeholder 

	if ((p.currentState.life -= data.initialState.life.second * dt) <= 0.f)
	{
		p.currentState.life = 0.f;
		p.camDist = -floatMax; 
	}
		
}

// -----------------------------------------------------------------
inline void ComponentParticleEmitter::SpeedUpdate(Particle& p, float dt)
{
	// Add the speed to the particle transform pos. Update the billboard too. Gravity? Yet another variable in the emitter xd
	auto pos = p.transf.globalMatrix.TranslatePart();
	p.transf.globalMatrix.SetTranslatePart(pos += (p.currentState.randomData.speed.IsFinite()) ? (p.currentState.randomData.speed * dt) : (data.initialState.speed.second * dt));

	// Update camera distance
	p.camDist = (p.transf.globalMatrix.TranslatePart() - camMatrix.TranslatePart()).Length();

	// Update Billboard
	//p.billboard.Update(camMatrix, FreeBillBoard::Alignment::world, p.transf);
}

// -----------------------------------------------------------------
inline void ComponentParticleEmitter::ColorUpdate(Particle& p, float dt)
{

	float4 initVal = (data.emissionData.randomColor.first) ? p.currentState.randomData.color : data.initialState.color.first; 
	float4 endVal = data.initialState.color.second; 

	if (endVal.IsFinite() == false)
		return; 

	float lifePercentatge = 1 - (p.currentState.life / data.initialState.life.first); 
	for (int i = 0; i < 4; ++i)
		p.currentState.color[i] = initVal[i] + lifePercentatge * (endVal[i] - initVal[i]); 

    // c = init + inverse percentage * range 
}


// ----------------------------------------------------------------- [Utilities]
float3 ComponentParticleEmitter::GetRandomRange(std::variant<float3, std::pair<float3, float3>> ranges)
{
	float3 ret = float3::zero;

	if (ranges.index() == 0)
	{
		auto range = std::get<float3>(ranges);
		ret.x = std::get<float>(RNG::GetRandomValue(-range.x / 2, range.x / 2));
		ret.y = std::get<float>(RNG::GetRandomValue(-range.y / 2, range.y / 2));
		ret.z = std::get<float>(RNG::GetRandomValue(-range.z / 2, range.z / 2));
	}
	else
	{
		auto range = std::get<std::pair<float3, float3>>(ranges);
		ret.x = std::get<float>(RNG::GetRandomValue(range.first.x, range.second.x));
		ret.y = std::get<float>(RNG::GetRandomValue(range.first.y, range.second.y));
		ret.z = std::get<float>(RNG::GetRandomValue(range.first.z, range.second.z));
	}

	return ret;
}

float4 ComponentParticleEmitter::GetRandomRange4(std::variant<float4, std::pair<float4, float4>> ranges)
{
	float4 ret = float4::zero;

	if (ranges.index() == 0)
	{
		auto range = std::get<float4>(ranges);
		ret.x = std::get<float>(RNG::GetRandomValue(-range.x / 2, range.x / 2));
		ret.y = std::get<float>(RNG::GetRandomValue(-range.y / 2, range.y / 2));
		ret.z = std::get<float>(RNG::GetRandomValue(-range.z / 2, range.z / 2));
		ret.w = std::get<float>(RNG::GetRandomValue(-range.w / 2, range.w / 2));
	}
	else
	{
		auto range = std::get<std::pair<float4, float4>>(ranges);
		ret.x = std::get<float>(RNG::GetRandomValue(range.first.x, range.second.x));
		ret.y = std::get<float>(RNG::GetRandomValue(range.first.y, range.second.y));
		ret.z = std::get<float>(RNG::GetRandomValue(range.first.z, range.second.z));
		ret.w = std::get<float>(RNG::GetRandomValue(range.first.w, range.second.w));
	}

	return ret;
}
