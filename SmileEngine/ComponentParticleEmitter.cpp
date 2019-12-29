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
#include "SmileFileSystem.h"
#include "ResourceTexture.h"
#include "SmileGameTimeManager.h"

// TODO: copy the initial values! Maybe have an instance of "initialValues" predefined too for the default ctor 


ComponentParticleEmitter::ComponentParticleEmitter(GameObject* parent)
{
	type = COMPONENT_TYPE::EMITTER;
	SetName("Emitter"); 

	SetupMesh();

	particles.resize(data.emissionData.maxParticles);

	pVariableFunctions.push_back(&ComponentParticleEmitter::LifeUpdate);
	pVariableFunctions.push_back(&ComponentParticleEmitter::SpeedUpdate);
	pVariableFunctions.push_back(&ComponentParticleEmitter::SizeUpdate);
	pVariableFunctions.push_back(&ComponentParticleEmitter::ColorUpdate);
	pVariableFunctions.push_back(&ComponentParticleEmitter::AnimUpdate);
}

ComponentParticleEmitter::ComponentParticleEmitter(GameObject* parent, AllData data) : data(data)
{
	type = COMPONENT_TYPE::EMITTER;
	SetName("Emitter");

	
	// 1) Push functions --> Only if the variable changes on time
	PushFunctions();

	// 2) Get resources   
	SetupMesh();
	SetupTexture(); 
	
	// 3) Resize the particles buffer   
	particles.resize(this->data.emissionData.maxParticles);
}

void ComponentParticleEmitter::PushFunctions()
{
	auto initialState = this->data.initialState;

	pVariableFunctions.push_back(&ComponentParticleEmitter::LifeUpdate);
	pVariableFunctions.push_back(&ComponentParticleEmitter::SpeedUpdate);
	pVariableFunctions.push_back(&ComponentParticleEmitter::SizeUpdate);
	pVariableFunctions.push_back(&ComponentParticleEmitter::ColorUpdate);
	pVariableFunctions.push_back(&ComponentParticleEmitter::AnimUpdate);

}

void ComponentParticleEmitter::SetupMesh()
{

	mesh = DBG_NEW ResourceMeshPlane(dynamic_cast<RNG*>(App->utilities->GetUtility("RNG"))->GetRandomUUID(), ownMeshType::plane, "Default", float4(1, 0, 0, 0.3f));
	App->resources->resources.insert(std::pair<SmileUUID, Resource*>(mesh->GetUID(), (Resource*)mesh));

	App->resources->UpdateResourceReferenceCount(mesh->GetUID(), particles.size());
}

void ComponentParticleEmitter::SetupTexture()
{
	if (this->data.emissionData.texPath != "empty")
	{
		texture = (ResourceTexture*)App->resources->GetResourceByPath(this->data.emissionData.texPath.c_str());
		if (texture == nullptr)
		{
			texture = (ResourceTexture*)App->resources->CreateNewResource(RESOURCE_TEXTURE, this->data.emissionData.texPath.c_str());
			texture->LoadOnMemory(this->data.emissionData.texPath.c_str());
		}

		this->data.initialState.tex.first = true;

		// Animation 
		if (this->data.initialState.tex.second > 0.f) // Check anim speed 
		{
			pVariableFunctions.push_back(&ComponentParticleEmitter::AnimUpdate);

		}

		App->resources->UpdateResourceReferenceCount(texture->GetUID(), particles.size());
	}

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

// -----------------------------------------------------------------
void ComponentParticleEmitter::Update(float dt)
{
	// Check expire time
	if(data.emissionData.expireTime > 0.f)
		if ((data.emissionData.totalTime += dt) >= data.emissionData.expireTime)
		{
			if (destroyOnFinish == false)
			{
				Disable();
				data.emissionData.totalTime = 0.f;
				data.emissionData.expireTime = 0.f;
			}
			else
				App->object_manager->toDestroy.push_back(GetParent()); 
		
			return; 
		}
	

	// Loop particles. Quickly discard inactive ones. Execute only needed functions 
	for (int i = 0; i < particles.size(); ++i)
		if(particles.at(i).currentState.life > 0.f)
			for (auto func = pVariableFunctions.begin(); func != pVariableFunctions.end(); ++func)
				(this->*(*func))(particles.at(i), dt);
		
	// Spawn new particles
	if (data.emissionData.burstTime > 0.f)
		BurstAction(dt);
	else
		DefaultSpawnAction(dt); 
		
}

// -----------------------------------------------------------------
void ComponentParticleEmitter::BurstAction(float dt)
{
	static bool burst = false;

	if ((data.emissionData.currentBurstTime += dt) >= data.emissionData.burstTime)
	{
		data.emissionData.currentBurstTime = 0;
		burst = !burst;
	}

	if (burst == false)
		DefaultSpawnAction(dt); 
}

// -----------------------------------------------------------------
void ComponentParticleEmitter::DefaultSpawnAction(float dt)
{
	if ((data.emissionData.currenTime += dt) > data.emissionData.time)
		SpawnParticle();
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
				p.currentState.needTileUpdate,
			(data.initialState.tex.first) ? texture : nullptr,
				data.blendmode, p.currentState.transparency, p.currentState.color,
				((data.initialState.tex.second > 0.f) ? p.currentState.tileIndex : INFINITE));

	drawParticles.clear();
}


// -----------------------------------------------------------------
inline static int FindAvailableParticleIndex(std::vector<Particle>& particles, uint& lastUsedParticle)
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
	p.currentState.transparency = data.initialState.transparency;
	p.currentState.life = data.initialState.life.first;
	p.currentState.size = data.initialState.size.first;
	auto scale = float3::FromScalar(p.currentState.size);
	 
	// Initial speed and color can be random:
	bool randomC = data.emissionData.randomColor;
	p.currentState.color = (randomC) ? (p.currentState.randomData.color = GetRandomRange4(std::pair(float4::zero, float4::one))) : data.initialState.color.first;
	bool randomS = data.emissionData.randomSpeed.first; 
	
	if (randomS == false) {
		p.currentState.speed = data.initialState.speed;
	}
	else 
	{
		if ((data.emissionData.randomSpeed.second.second.IsFinite())) {
			p.currentState.randomData.speed = GetRandomRange(data.emissionData.randomSpeed.second);
		}
		else
		{
			p.currentState.randomData.speed = GetRandomRange(data.emissionData.randomSpeed.second.first);
		}
	}
		 
	// 4) Set particle Transform
	p.transf.parentMatrix = GetParent()->GetTransform()->GetGlobalMatrix(); 
	p.transf.UpdateGlobalMatrix(math::float4x4::FromTRS(GetSpawnPos(), float4x4::identity, scale));
}

float3 ComponentParticleEmitter::GetSpawnPos()
{
	auto pos = GetParent()->GetTransform()->GetGlobalPosition(); 
	auto copy = pos; 
	switch (data.emissionData.shape)
	{
	case emmissionShape::CIRCLE:
	{
		pos += GetRandomRange(data.emissionData.spawnRadius);
		pos.y = copy.y; 
		break; 
	}
	case emmissionShape::SPHERE:
	{
		pos += GetRandomRange(data.emissionData.spawnRadius);
		break;
	}
	case emmissionShape::CONE:
	{
		
		break;
	}
	
	default:
		break;
	}

	return pos;
}

// ----------------------------------------------------------------- Update Values
inline void ComponentParticleEmitter::LifeUpdate(Particle& p, float dt)
{
	p.currentState.currentLifeTime += dt; 

	if ((p.currentState.life -= data.initialState.life.second * dt) <= 0.f)
	{
		p.currentState.life = p.currentState.currentLifeTime = 0.f;
		p.camDist = -floatMax; 
	}
		
}

// -----------------------------------------------------------------
inline void ComponentParticleEmitter::SpeedUpdate(Particle& p, float dt)
{
	// Add the speed to the particle transform pos. Update the billboard too. Gravity? Yet another variable in the emitter xd
	auto pos = p.transf.globalMatrix.TranslatePart();
	float3 delta = (p.currentState.randomData.speed.IsFinite()) ? (p.currentState.randomData.speed * dt) : (data.initialState.speed * dt);
	if (data.emissionData.gravity)
		delta += float3(0, -GLOBAL_GRAVITY * p.currentState.currentLifeTime * dt, 0); 

	p.transf.globalMatrix.SetTranslatePart(pos += delta);

	// Update camera distance
	auto camMatrix = App->scene_intro->gameCamera->GetViewMatrixF(); 
	p.camDist = (p.transf.globalMatrix.TranslatePart() - camMatrix.TranslatePart()).Length();

	// Update Billboard
	p.billboard.Update(camMatrix, FreeBillBoard::Alignment::world, nullptr, p.transf); 
}

// -----------------------------------------------------------------
inline void ComponentParticleEmitter::ColorUpdate(Particle& p, float dt)
{
	if (data.emissionData.randomColor)
		return;

	float4 initVal = (data.emissionData.randomColor) ? p.currentState.randomData.color : data.initialState.color.first; 
	float4 endVal = data.initialState.color.second; 

	if (endVal.IsFinite() == false)
		return; 

	float lifePercentatge = 1 - (p.currentState.life / data.initialState.life.first); 
	for (int i = 0; i < 4; ++i)
		p.currentState.color[i] = initVal[i] + lifePercentatge * (endVal[i] - initVal[i]); 

    // c = init + inverse percentage * range 
}

// -----------------------------------------------------------------
inline void ComponentParticleEmitter::AnimUpdate(Particle& p, float dt)
{
	if (data.initialState.tex.second == 0.f)
		return; 
	
	if ((p.currentState.lastTileframe += dt) >= data.initialState.tex.second)
	{
		p.currentState.tileIndex = ((p.currentState.tileIndex + 1) < mesh->tileData->maxTiles -1) ?
			(p.currentState.tileIndex + 1) : 0; 
		p.currentState.lastTileframe = 0.f; 
		p.currentState.needTileUpdate = true;
	};

}


// -----------------------------------------------------------------
inline void ComponentParticleEmitter::SizeUpdate(Particle& p, float dt)
{
	auto t = p.transf.GetGlobalMatrix();
	float initVal = data.initialState.size.first;
	float endVal = data.initialState.size.second;
	float lifePercentatge = 1 - (p.currentState.life / data.initialState.life.first);
	p.currentState.size = initVal + lifePercentatge * (endVal - initVal);
	auto sc = float3::FromScalar(p.currentState.size); 
	p.transf.ChangeScale(sc); 
}

// ----------------------------------------------------------------- [Utilities]
float3 ComponentParticleEmitter::GetRandomRange(std::variant<float3, std::pair<float3, float3>> ranges)
{
	float3 ret = float3::zero;

	if (ranges.index() == 0)
	{
		auto range = std::get<float3>(ranges);
		range.x = math::Abs(range.x); 
		range.y = math::Abs(range.y);
		range.z = math::Abs(range.z);

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

// -----------------------------------------------------------------
void ComponentParticleEmitter::SetNewTexture(const char* path)
{
	if (App->fs->Exists(path) == false)
		return; 

	auto newTexture = (ResourceTexture*)App->resources->GetResourceByPath(path);
	if (newTexture == nullptr)
		newTexture = (ResourceTexture*)App->resources->CreateNewResource(RESOURCE_TEXTURE, path);
	
	if (newTexture == nullptr)
		return;

	if (newTexture)
	{
		texture = newTexture; 
		App->resources->UpdateResourceReferenceCount(texture->GetUID(), -particles.size());
	}

	this->data.emissionData.texPath = path;
	texture->LoadOnMemory(this->data.emissionData.texPath.c_str());
		
	this->data.initialState.tex.first = true;
	App->resources->UpdateResourceReferenceCount(texture->GetUID(), particles.size());
}


// -----------------------------------------------------------------
void ComponentParticleEmitter::SetMaxParticles(uint maxParticles)
{
	particles.resize(data.emissionData.maxParticles = maxParticles); 
	if (lastUsedParticle > particles.size())
		lastUsedParticle = particles.size(); 
}

void ComponentParticleEmitter::OnSave(rapidjson::Writer<rapidjson::StringBuffer>& writer)
{
	
	writer.StartObject();
	writer.Key("Emitter");

	writer.StartObject();
	writer.Key("Active");
	writer.Bool(active);
	
	writer.Key("Max Particles");
	writer.Uint(data.emissionData.maxParticles);

	writer.Key("Last Used Particle");
	writer.Uint(lastUsedParticle);
	   
	writer.Key("Bounding Box Radius");
	writer.Double(GetParent()->GetBoundingData().OBB.Size().Length());

	
	writer.Key("Blend Mode");
	bool alphaBlend = (data.blendmode == blendMode::ALPHA_BLEND) ? true : false;
	if (alphaBlend == true)
	{
		writer.String("ALPHA BLEND");
	}
	else
	{
		writer.String("ADDITIVE");
	}
	bool oneRange = (data.emissionData.randomSpeed.second.second.IsFinite()) ? false : true;
	writer.Key("One Range");
	writer.Bool(oneRange);


	writer.Key("Has Random Speed");
	writer.Bool(data.emissionData.randomSpeed.first);

	writer.Key("Speed"); 
	writer.StartArray(); 
	if (data.emissionData.randomSpeed.first == true)
	{
		
			// Two ranges
			writer.Key("First Range");
			writer.StartArray();
			writer.Double(data.emissionData.randomSpeed.second.first.x);
			writer.Double(data.emissionData.randomSpeed.second.first.y);
			writer.Double(data.emissionData.randomSpeed.second.first.z);
			writer.EndArray();
			

			auto value = float3(666); 
			if (oneRange == false)
				value = data.emissionData.randomSpeed.second.second; 
			

				writer.Key("Second Range");
				writer.StartArray();
				writer.Double(value.x);
				writer.Double(value.y);
				writer.Double(value.z);
				writer.EndArray();
		
		
		
	}
	else
	{
		writer.Double(data.initialState.speed.x);
		writer.Double(data.initialState.speed.y);
		writer.Double(data.initialState.speed.z);
	}
	writer.EndArray();
	
	writer.Key("Gravity");
	writer.Bool(data.emissionData.gravity);

	writer.Key("Initial Life");
	writer.Double(data.initialState.life.first);
	writer.Key("Life Decrease");
	writer.Double(data.initialState.life.second);
	
	writer.Key("Has Random Color");
	writer.Bool(data.emissionData.randomColor);

	if (data.emissionData.randomColor == false)
	{
		auto value = float4(666,666,666,666);
		auto value2 = float4(666,666,666,666);
		if (data.initialState.color.first.IsFinite())
		{
			value = data.initialState.color.first;
			
		}
		if (data.initialState.color.second.IsFinite())
		{
			value2 = data.initialState.color.second;

		}
		writer.Key("Initial Color");
		writer.StartArray();
		writer.Double(value.x);
		writer.Double(value.y);
		writer.Double(value.z);
		writer.Double(value.w);
		writer.EndArray();
		writer.Key("Final Color");
		writer.StartArray();
		writer.Double(value2.x);
		writer.Double(value2.y);
		writer.Double(value2.z);
		writer.Double(value2.w);
		writer.EndArray();


	}
	
	writer.Key("Initial Particle Size");
	writer.Double(data.initialState.size.first);
	writer.Key("Final Particle Size");
	writer.Double(data.initialState.size.second);

	writer.Key("Spawn Shape");

	if (data.emissionData.shape == emmissionShape::CIRCLE)
	{
		writer.String("CIRCLE");
	}
	if (data.emissionData.shape == emmissionShape::CONE)
	{
		writer.String("CONE");
	}
	if (data.emissionData.shape == emmissionShape::SPHERE)
	{
		writer.String("SPHERE");
	}

	writer.Key("Emitter Spawn Time");
	writer.Double(data.emissionData.time);

	writer.Key("Burst Time");
	writer.Double(data.emissionData.burstTime);

	writer.Key("Current Time");
	writer.Double(data.emissionData.currenTime);

	writer.Key("Current Burst Time");
	writer.Double(data.emissionData.currentBurstTime);

	writer.Key("Total Time");
	writer.Double(data.emissionData.totalTime);
	
	writer.Key("Expiration Time");
	writer.Double(data.emissionData.expireTime);

	writer.Key("Emitter Spawn Radius");
	writer.StartArray();
	writer.Double(data.emissionData.spawnRadius.x);
	writer.Double(data.emissionData.spawnRadius.y);
	writer.Double(data.emissionData.spawnRadius.z);
	writer.EndArray();

	
	writer.Key("Texture Active");
	writer.Bool(data.initialState.tex.first);
	writer.Key("Texture Path");
	writer.String(data.emissionData.texPath.c_str());
	writer.Key("Transparency");
	writer.Double(data.initialState.transparency);
	
	writer.Key("Texture Animation Speed");
	writer.Double(data.initialState.tex.second);

	writer.Key("Number of Tiles");
	writer.Int(mesh->tileData->maxTiles);
	writer.Key("Number of Rows");
	writer.Int(mesh->tileData->nRows);
	writer.Key("Number of Columns");
	writer.Int(mesh->tileData->nCols);

	
	


	// Particles
	writer.Key("Particles");
	writer.StartArray();

	for (int i = 0; i < data.emissionData.maxParticles; ++i)
	{
		writer.StartObject();
		writer.Key("Particle");
		writer.StartObject();

		writer.Key("Active");
		writer.Bool(particles.at(i).currentState.active);
		
		writer.Key("Speed");
		writer.StartArray();
		writer.Double(particles.at(i).currentState.speed.x);
		writer.Double(particles.at(i).currentState.speed.y);
		writer.Double(particles.at(i).currentState.speed.z);
		writer.EndArray();

		writer.Key("Life");
		writer.Double(particles.at(i).currentState.life);

		writer.Key("Current Life Time");
		writer.Double(particles.at(i).currentState.currentLifeTime);
		

		float4 colorValue(666, 666, 666, 666); 
		if (particles.at(i).currentState.color.IsFinite())
			colorValue = particles.at(i).currentState.color;

		writer.Key("Color");
		writer.StartArray();
		writer.Double(colorValue.x);
		writer.Double(colorValue.y);
		writer.Double(colorValue.z);
		writer.Double(colorValue.w);
		writer.EndArray();


		writer.Key("Size");
		writer.Double(particles.at(i).currentState.size);
		
		writer.Key("Transparency");
		writer.Double(particles.at(i).currentState.transparency);
		
		writer.Key("Tile Index");
		writer.Uint(particles.at(i).currentState.tileIndex);
	
		writer.Key("Last Tile Frame");
		writer.Double(particles.at(i).currentState.lastTileframe);
		
		writer.Key("Need Tile Update");
		writer.Bool(particles.at(i).currentState.needTileUpdate);
		
		writer.Key("Random Speed");
		writer.StartArray();
		auto value = float3(666);
		if (particles.at(i).currentState.randomData.speed.IsFinite() == true)
		{
			value.x = particles.at(i).currentState.randomData.speed.x;
			value.y = particles.at(i).currentState.randomData.speed.y;
			value.z = particles.at(i).currentState.randomData.speed.z;
		}
		writer.Double(value.x);
		writer.Double(value.y);
		writer.Double(value.z);
		writer.EndArray();



		writer.Key("Random Color");
		writer.StartArray();
		auto valueC = float4(666,666,666,666);
		if (particles.at(i).currentState.randomData.color.IsFinite() == true)
		{
			valueC.x = particles.at(i).currentState.randomData.color.x;
			valueC.y = particles.at(i).currentState.randomData.color.y;
			valueC.z = particles.at(i).currentState.randomData.color.z;
			valueC.w = particles.at(i).currentState.randomData.color.w;
		}
		writer.Double(valueC.x);
		writer.Double(valueC.y);
		writer.Double(valueC.z);
		writer.Double(valueC.w);
		writer.EndArray();

		writer.Key("Cam Distance");
		writer.Double(particles.at(i).camDist);

		writer.Key("Global Matrix");
		float* m = particles.at(i).transf.globalMatrix.ptr();
		writer.StartArray();
		for (int i = 0; i < 16; ++i)
		{
			writer.Double(m[i]);
		}
		writer.EndArray();

		writer.Key("Local Matrix");
		float* mLocal = particles.at(i).transf.localMatrix.ptr();
		writer.StartArray();
		for (int i = 0; i < 16; ++i)
		{
			writer.Double(mLocal[i]);
		}
		writer.EndArray();
		writer.EndObject();
		writer.EndObject();
	}
	writer.EndArray();


	writer.EndObject();
	writer.EndObject();
	
}
