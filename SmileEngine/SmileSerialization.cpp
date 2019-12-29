#include "SmileSerialization.h"
#include "SmileFBX.h"
#include "JSONParser.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"
#include "GameObject.h"
#include <fstream>
#include "JSONParser.h"
#include "SmileUtilitiesModule.h"
#include "ComponentParticleEmitter.h"
#include "ResourceMeshPlane.h"
#include <filesystem>
#include "SmileApp.h"
#include "ComponentTypes.h"

SmileSerialization::SmileSerialization(SmileApp* app, bool start_enabled) : SmileModule(app, start_enabled)
{

}

SmileSerialization::~SmileSerialization()
{
}

bool SmileSerialization::SaveScene()
{
	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	
	SaveSceneNode(App->scene_intro->rootObj, writer);

	const char* output = buffer.GetString();
	std::string dirPath;
	App->fs->SaveUnique(dirPath, output, buffer.GetSize(), SCENE_FOLDER, "scene", "json");

	return false;
}

bool SmileSerialization::SaveSceneNode(GameObject* obj, rapidjson::Writer<rapidjson::StringBuffer>& writer)
{

	auto transf = obj->GetTransform();
	
	std::vector <GameObject*>children = obj->GetImmidiateChildren();

	float3 position = transf->GetPosition();
	float3 scale = transf->GetScale();
	Quat rotation = transf->GetRotation();

	// window 
	writer.StartObject();
	writer.Key("GameObject");

	writer.StartObject();

	// Model variables
	writer.Key("UID");
	writer.Int(obj->GetID());

	GameObject* parent = obj->GetParent();
	if (parent)
	{
		writer.Key("Parent ID");
		writer.Int(parent->GetID());
	}

	writer.Key("Name");
	writer.String(obj->GetName().c_str());

	writer.Key("Selected");
	writer.Bool((App->scene_intro->selectedObj == obj) ? true : false);
	
	writer.Key("Static");
	writer.Bool(obj->GetStatic());

	// - - - - - - - - - - - - - - - - - - - - Components
	writer.Key("Components");
	writer.StartArray();

	// - - - - - - - - - - - - Transform
	writer.StartObject();
	writer.Key("Transform");
	writer.StartObject();

	// Pos
	writer.Key("Position");
	writer.StartArray();

	writer.Double(position.x);
	writer.Double(position.y);
	writer.Double(position.z);

	writer.EndArray();

	// Rot
	writer.Key("Rotation");
	writer.StartArray();

	writer.Double(rotation.x);
	writer.Double(rotation.y);
	writer.Double(rotation.z);
	writer.Double(rotation.w);

	writer.EndArray();

	// Scale
	writer.Key("Scale");
	writer.StartArray();

	writer.Double(scale.x);
	writer.Double(scale.y);
	writer.Double(scale.z);

	writer.EndArray();

	writer.EndObject();
	writer.EndObject(); 

	// - - - - - - - - - - - - (end tranform)

	// - - - - - - - - - - - - Mesh
	auto mesh = obj->GetMesh();
	if (mesh)
	{
		auto material = obj->GetMaterial();

		writer.StartObject();
		writer.Key("Mesh");

		writer.StartObject();

	
		 
		writer.Key("path");
		writer.String(App->fbx->SaveMesh(mesh->GetResourceMesh(), obj).c_str());

		writer.Key("materialPath");

		if (material)
			writer.String(App->fbx->SaveMaterial(material->GetTextureData()->path.c_str()).c_str());
		else
			writer.String("Empty");

		writer.Key("Selected");
		writer.Bool((App->scene_intro->selected_mesh == mesh) ? true : false);

		writer.EndObject();


		writer.EndObject();
	}
	// - - - - - - - - - - - - (end mesh)

	// - - - - - - - - - - - - Camera
	auto camera = obj->GetCamera(); 
	if (camera)
	{
		writer.StartObject();
		writer.Key("Camera");
		writer.StartObject();

		auto data = camera->GetRenderingData(); 

		writer.Key("FovY"); 
		writer.Double(data.fovYangle); 
		writer.Key("Near Plane Distance");
		writer.Double(data.pNearDist);
		writer.Key("Far Plane Distance");
		writer.Double(data.pFarDist);
		writer.Key("Ratio");
		writer.Double(data.ratio);

		auto ref = camera->Reference; 
		writer.Key("Reference");
		writer.StartArray();

		writer.Double(ref.x);
		writer.Double(ref.y);
		writer.Double(ref.z);

		writer.EndArray();

		// Game vs Debug camera and Target 
		writer.Key("Is Debug");
		writer.Bool(App->scene_intro->debugCamera == camera); 

		writer.Key("Is Target");
		writer.Bool(App->renderer3D->targetCamera == camera);

		writer.EndObject();
		writer.EndObject();


	}

	// - - - - - - - - - - - - (end camera)

	//Component emitter
	auto emitter = obj->GetEmitter();
	if(emitter)
		emitter->OnSave(writer);
	
	
	//end component emitter

	writer.EndArray();
	// - - - - - - - - - - - - - - - - - - - - (end Components)
    // - - - - - - - - - - - - - - - - - - - - Children
	writer.Key("Children");
	writer.StartArray();
	
	if (children.size() > 0) 
	{
		for (auto& child : children)
		{
			SaveSceneNode(child, writer);
		}

	}
	writer.EndArray();


	writer.EndObject();

	writer.EndObject(); // end gameObject object

	return false;
}

GameObject* SmileSerialization::LoadSceneNode(GameObject* parent, rapidjson::Value& mynode, rapidjson::Document& doc)
{
	// Create object
	GameObject* obj = (parent) ? DBG_NEW GameObject() : App->scene_intro->rootObj;

	// General data
	if(parent)
		obj->SetParent(parent);


	obj->randomID = (SmileUUID)mynode["UID"].GetInt64();  // Set it (already in constructor, careful)
	obj->SetName(mynode["Name"].GetString());
	bool selected = mynode["Selected"].GetBool();
	if (selected)
		App->scene_intro->selectedObj = obj;
 
	// Components
	// We must assume transform:
	auto compArray = mynode["Components"].GetArray(); 
	uint i = 0; 
	for (auto& comp : compArray) // component array node
	{
		// get the name of the memeber inside the node
		for (rapidjson::Value::ConstMemberIterator iter = comp.MemberBegin(); iter != comp.MemberEnd(); ++iter)
		{
			auto name =  iter->name.GetString();
			auto object = iter->value.GetObjectA(); 

			switch (componentTypeMap.at(name))
			{
			case TRANSFORM: 
			{
				auto pos = object["Position"].GetArray(); 
				auto rot = object["Rotation"].GetArray();
				auto scale = object["Scale"].GetArray();
			     
				float3 realPos = float3(0, 0, 0), realScale = float3(0, 0, 0); 
				math::Quat realRot = Quat(); float captureRot[4]; 

				for (rapidjson::SizeType i = 0; i < pos.Size(); i++) 
					realPos[i] = pos[i].GetDouble();
			
				for (rapidjson::SizeType i = 0; i < scale.Size(); i++)
					realScale[i] = scale[i].GetDouble();

				for (rapidjson::SizeType i = 0; i < rot.Size(); i++)
					captureRot[i] = rot[i].GetDouble(); 
				realRot = Quat(captureRot[0], captureRot[1], captureRot[2], captureRot[3]);

				obj->AddComponent((Component*)DBG_NEW ComponentTransform(math::float4x4::FromTRS(realPos, realRot, realScale)));

				break; 
			}

			case MESH:
			{
				auto meshPath = object["path"].GetString();
				auto materialPath = object["materialPath"].GetString();
				auto selectedMesh = object["Selected"].GetBool();

				obj->AddComponent(App->fbx->LoadMesh(meshPath));
				if (materialPath != "empty")
					App->fbx->AssignTextureToObj(materialPath, obj);
				if (selectedMesh)
					App->scene_intro->selected_mesh = obj->GetMesh();
				break;
			}

			case CAMERA:
			{
				auto fovY = object["FovY"].GetDouble();
				auto nPlaneDist = object["Near Plane Distance"].GetDouble();
				auto fPlaneDist = object["Far Plane Distance"].GetDouble();
				auto ratio = object["Ratio"].GetDouble();
				auto reference = object["Reference"].GetArray();
				float refArray[3]; 
				for (rapidjson::SizeType i = 0; i < reference.Size(); i++)
					refArray[i] = reference[i].GetDouble();
				vec3 referenceFloat3 = vec3(refArray[0], refArray[1], refArray[2]);


				// Debug vs Game and Target
				bool debug = object["Is Debug"].GetBool(); 
				bool target = object["Is Target"].GetBool();

				renderingData data;
				data.pNearDist = nPlaneDist; 
				data.pFarDist = fPlaneDist; 
				data.fovYangle = fovY; 
				data.ratio = ratio; 
				ComponentCamera* cam = DBG_NEW ComponentCamera(obj, referenceFloat3, data);
				obj->AddComponent(cam);

				if (debug)
					App->scene_intro->debugCamera = cam;
				else
					App->scene_intro->gameCamera = cam;  

				if (target)
					App->renderer3D->SetTargetCamera(cam); 
					

				break;
			}

			case EMITTER:
			{
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  Emitter
			    bool active = object["Active"].GetBool(); 
				uint maxParticles = object["Max Particles"].GetInt(); 
				uint lastUsedParticle = object["Last Used Particle"].GetInt(); 
				float boundingBoxRadius = object["Bounding Box Radius"].GetFloat(); 
				std::string blendModeString = object["Blend Mode"].GetString(); 
				blendMode blendMode = (blendModeString == "ALPHA BLEND") ? blendMode::ALPHA_BLEND : blendMode::ADDITIVE; 
				bool hasRandomSpeed = object["Has Random Speed"].GetBool(); 
				auto Speed = object["Speed"].GetArray();
				float3 SpeedFirstRange, SpeedSecondRange;
				if (hasRandomSpeed)
				{
					for (rapidjson::SizeType i = 0; i < Speed.Size(); i++)
					{
						const rapidjson::Value& range = Speed[i];

						if (i == 1)
						{
							for (rapidjson::SizeType i = 0; i < range.Size(); i++)
								SpeedFirstRange[i] = range[i].GetDouble();
						}
						else if (i == 3)
						{
							for (rapidjson::SizeType i = 0; i < range.Size(); i++)
								SpeedSecondRange[i] = range[i].GetDouble();
						}

					}
				}
				else
				{
					for (rapidjson::SizeType i = 0; i < Speed.Size(); i++)
						SpeedFirstRange[i] = Speed[i].GetDouble();
				}
				bool gravity = object["Gravity"].GetBool();
				float initialLife = object["Initial Life"].GetFloat();
				float lifeDecrease = object["Life Decrease"].GetFloat();
				bool hasRandomColor = object["Has Random Color"].GetBool();
				auto color1 = object["Initial Color"].GetArray();
				auto color2 = object["Final Color"].GetArray();
				float4 Color = float4::inf;
				float4 Color2 = float4::inf;
				if (hasRandomColor == false)
				{
					if(color1[0].GetDouble() == 666)
					{
						goto next;
					}
					for (rapidjson::SizeType i = 0; i < color1.Size(); i++)
						Color[i] = color1[i].GetDouble();
					for (rapidjson::SizeType i = 0; i < color2.Size(); i++)
						Color2[i] = color2[i].GetDouble();

				}
				next:
				float initialParticleSize = object["Initial Particle Size"].GetFloat(); 
				float finalParticleSize = object["Final Particle Size"].GetFloat();
				std::string spawnShapeString = object["Spawn Shape"].GetString(); 
				emmissionShape shape = emmissionShape::CIRCLE; 
				if(spawnShapeString ==  "SPHERE")
					shape = emmissionShape::SPHERE;
				else if (spawnShapeString == "CONE")
					shape = emmissionShape::CONE;
				float emitterSpawnTime = object["Emitter Spawn Time"].GetFloat(); 
				float burstTime = object["Burst Time"].GetFloat();
				float currentTime = object["Current Time"].GetFloat();
				float currentBurstTime = object["Current Burst Time"].GetFloat();
				float totalTime = object["Total Time"].GetFloat();
				float expirationTime = object["Expiration Time"].GetFloat();

				auto radius = object["Emitter Spawn Radius"].GetArray();
				float3 spawnRadius = float3(5.f); 
				for (rapidjson::SizeType i = 0; i < radius.Size(); i++)
					spawnRadius[i] = radius[i].GetFloat();
				bool textureActive = object["Texture Active"].GetBool();
				std::string texturePath = object["Texture Path"].GetString(); 
				float transp = object["Transparency"].GetFloat(); 
				float animSpeed  = object["Texture Animation Speed"].GetFloat();
				uint maxTiles = object["Number of Tiles"].GetInt();
				uint nRows = object["Number of Rows"].GetInt();
				uint nCols = object["Number of Columns"].GetInt();

				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  Emitter
				AllData data;
				data.blendmode = blendMode;
				//emission data
				data.emissionData.burstTime = burstTime;
				data.emissionData.totalTime = totalTime;
				data.emissionData.time = emitterSpawnTime;
				data.emissionData.currentBurstTime = currentBurstTime;
				data.emissionData.currenTime = currentTime;
				data.emissionData.expireTime = expirationTime;
				data.emissionData.gravity = gravity;
				data.emissionData.maxParticles = maxParticles;
				data.emissionData.randomColor = hasRandomColor;

				data.emissionData.randomSpeed.first = hasRandomSpeed;
				if (hasRandomSpeed) {
					data.emissionData.randomSpeed.second.first = SpeedFirstRange;
					if (SpeedSecondRange.x == 666.f) {
						data.emissionData.randomSpeed.second.second = float3::inf;
					}
				}
				data.emissionData.shape = shape;
				data.emissionData.spawnRadius = spawnRadius;
				data.emissionData.texPath = texturePath;
				//initial state
				if (Color.IsFinite() == true) {
					data.initialState.color.first = Color;
					data.initialState.color.second = Color2;
				}
				data.initialState.life.first = initialLife;
				data.initialState.life.second = lifeDecrease;
				data.initialState.size.first = initialParticleSize;
				data.initialState.size.second = finalParticleSize;

				if (hasRandomSpeed == false) {
					data.initialState.speed = SpeedFirstRange;
				}

				data.initialState.tex.first = textureActive;
				data.initialState.tex.second = animSpeed;
				data.initialState.transparency = transp;
				ComponentParticleEmitter* emitter = DBG_NEW ComponentParticleEmitter(obj, data);
				emitter->lastUsedParticle = lastUsedParticle;
				obj->ResizeBounding(boundingBoxRadius);
				emitter->active = active;
				emitter->mesh->tileData->maxTiles = maxTiles;
				emitter->mesh->tileData->nCols = nCols;
				emitter->mesh->tileData->nRows = nRows;

				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  Particles
				auto particles = object["Particles"].GetArray();
				int counter = 0;
				for (auto& particleNode : particles)
				{
					auto particle = particleNode["Particle"].GetObjectA();
					auto active = particle["Active"].GetBool();
					auto speed = particle["Speed"].GetArray();
					float3 Speed;
					for (rapidjson::SizeType i = 0; i < speed.Size(); i++)
						Speed[i] = speed[i].GetDouble();
					
					auto life = particle["Life"].GetFloat();
					auto currentLifeTime = particle["Current Life Time"].GetFloat();
					auto color = particle["Color"].GetArray();
					float4 Color;
					for (rapidjson::SizeType i = 0; i < color.Size(); i++)
						Color[i] = color[i].GetDouble();

					auto randomcolor = particle["Random Color"].GetArray();
					float4 RandomColor = float4::inf;

					if (randomcolor[0].GetDouble() != 666)
					{
						for (rapidjson::SizeType i = 0; i < randomcolor.Size(); i++)
							RandomColor[i] = randomcolor[i].GetDouble();
					}

					auto randomspeed = particle["Random Speed"].GetArray();
					float4 RandomSpeed = float4::inf;

					if (randomspeed[0].GetDouble() != 666)
					{
						for (rapidjson::SizeType i = 0; i < randomspeed.Size(); i++)
							RandomSpeed[i] = randomspeed[i].GetDouble();
					}

					

					auto camDistance = particle["Cam Distance"].GetFloat();
					auto size = particle["Size"].GetFloat();
					auto transparency = particle["Transparency"].GetFloat();
					auto tileIndex = particle["Tile Index"].GetInt();
					auto lastTileFrame = particle["Last Tile Frame"].GetFloat();
					auto needTileUpdate = particle["Need Tile Update"].GetBool();



					emitter->particles.at(i).currentState.active = active;
					emitter->particles.at(i).currentState.color = Color;
					emitter->particles.at(i).currentState.currentLifeTime = currentLifeTime;
					emitter->particles.at(i).currentState.lastTileframe = lastTileFrame;
					emitter->particles.at(i).currentState.life = life;
					emitter->particles.at(i).currentState.needTileUpdate = needTileUpdate;
					emitter->particles.at(i).currentState.size = size;
					emitter->particles.at(i).currentState.speed = Speed;
					emitter->particles.at(i).currentState.tileIndex = tileIndex;
					emitter->particles.at(i).currentState.transparency = transparency;
					emitter->particles.at(i).transf.globalMatrix = obj->GetParent()->GetTransform()->GetGlobalMatrix();

					counter++;
				}
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  Particles
				break; 
			}

			default:
				break;
			}
		}
	}
	
 


	// Children
	auto children_array = mynode["Children"].GetArray();
	if (children_array.Empty())
		return obj; 

	for (auto& child : children_array) 
		LoadSceneNode(obj, child["GameObject"], doc);
	
	return obj;
}

void SmileSerialization::LoadScene(const char* path, bool startup)
{	
	if (App->fs->Exists(path) == false)
		return; 

	rapidjson::Document doc;
	dynamic_cast<JSONParser*>(App->utilities->GetUtility("JSONParser"))->ParseJSONFile(path, doc);
	
	if (startup == false)
	{
		// 1) Clear Octree
		App->spatial_tree->CleanUp();
		// 2) Clear All Objects
		App->scene_intro->Reset();
		// 3) Reset current camera
		App->renderer3D->Reset();
	}

	// 4) Then Load
	rapidjson::Value& value = doc["GameObject"]; 
	LoadSceneNode(nullptr, value, doc)->Start();  // starts root 
	// 5) Afterwards, create Octree again
	App->spatial_tree->CreateOctree(math::AABB(float3(-100, -100, -100), float3(100, 100, 100)));
}
