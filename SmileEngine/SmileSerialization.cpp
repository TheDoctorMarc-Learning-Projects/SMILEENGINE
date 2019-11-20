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

	//static, bounding box

	// - - - - - - - - - - - - Mesh
	
	
	if (obj->GetMesh())
	{
		writer.StartObject();
		writer.Key("Mesh");

		writer.StartObject();

		auto material = obj->GetMaterial();
		auto mesh = obj->GetMesh();


		writer.Key("path");
		writer.String(App->fbx->SaveMesh(mesh->GetMeshData(), obj).c_str());

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
	GameObject* obj = DBG_NEW GameObject();

	// General data
	if(parent)
		obj->SetParent(parent);

	obj->randomID = (SmileUUID)mynode["UID"].GetInt64();  // Set it (already in constructor, careful)
	obj->SetName(mynode["Name"].GetString());
	bool selected = mynode["Selected"].GetBool();
	if (selected)
		App->scene_intro->selectedObj = obj;
	obj->SetStatic(mynode["Static"].GetBool()); 

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
				auto meshpath = object["path"].GetString();
				auto materialpath = object["materialPath"].GetString();
				auto selectedMesh = object["Selected"].GetBool();
				obj->AddComponent(App->fbx->LoadMesh(obj->GetMesh()->GetMeshData(), meshpath));
				if (materialpath != "empty")
					App->fbx->AssignTextureToObj(materialpath, obj);
				if (selectedMesh)
					App->scene_intro->selected_mesh = obj->GetMesh();
				break;
			}

			case CAMERA:
			{
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

void SmileSerialization::LoadScene(const char* path)
{	
	rapidjson::Document doc;
	dynamic_cast<JSONParser*>(App->utilities->GetUtility("JSONParser"))->ParseJSONFile(path, doc);
	

	// When Loading: 
	// 1) Clear Octree
	// 2) Clear All Objects
	// 3) Then Load
	App->scene_intro->rootObj = LoadSceneNode(nullptr, doc["GameObject"], doc);
	App->scene_intro->rootObj->Start(); // start all children (compute bounding etc) 
	// 4) Afterwards, create Octree again

}
