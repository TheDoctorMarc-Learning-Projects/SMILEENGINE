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

	writer.StartArray();

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
	

	// - - - - - - - - - - - - Transform
	writer.Key("Transform");
	writer.StartArray();
	writer.StartObject();

	// Pos
	writer.Key("Position");
	writer.StartArray();

	writer.Int(position.x);
	writer.Int(position.y);
	writer.Int(position.z);

	writer.EndArray();

	// Rot
	writer.Key("Rotation");
	writer.StartArray();

	writer.Int(rotation.x);
	writer.Int(rotation.y);
	writer.Int(rotation.z);
	writer.Int(rotation.w);

	writer.EndArray();

	// Scale
	writer.Key("Scale");
	writer.StartArray();

	writer.Int(scale.x);
	writer.Int(scale.y);
	writer.Int(scale.z);

	writer.EndArray();

	writer.EndObject();
	writer.EndArray();

	// - - - - - - - - - - - - (end tranform)

	//static, bounding box

	
	
	//Meshes


	writer.Key("Mesh");

	writer.StartArray();


	
	if (obj->GetMesh())
	{
			writer.StartObject();
			

			auto material = obj->GetMaterial();
			auto mesh = obj->GetMesh();


			writer.Key("path");
			writer.String(App->fbx->SaveMesh(mesh->GetMeshData(), obj).c_str());

			writer.Key("materialPath");

			if (material)
				writer.String(App->fbx->SaveMaterial(material->GetTextureData()->path.c_str()).c_str());
			else
				writer.String("empty");

			writer.EndObject();

	}
	
	writer.EndArray();


	//children

	if (children.size() > 0) {
		
		writer.Key("Children");
		writer.StartArray();
		//writer.StartObject();
		for (auto& child : children)
		{
			SaveSceneNode(child, writer);
		}
		//writer.EndObject();
		writer.EndArray();
		
	}
	writer.EndObject();

	writer.EndArray();

	writer.EndObject();

	return false;
}

bool SmileSerialization::LoadSceneNode(GameObject* go, const char* path)
{
	rapidjson::Document doc;
	dynamic_cast<JSONParser*>(App->utilities->GetUtility("JSONParser"))->ParseJSONFile(path, doc);
	int id = rapidjson::GetValueByPointer(doc, "/GameObject/0/UID")->GetInt();
	int parent_id = rapidjson::GetValueByPointer(doc, "/GameObject/0/Parent ID")->GetInt();

	std::string name = rapidjson::GetValueByPointer(doc, "/GameObject/0/Name")->GetString();
	bool selected = rapidjson::GetValueByPointer(doc, "/GameObject/0/Selected")->GetBool();

	
	return false;
}

GameObject* SmileSerialization::LoadScene(const char* path)
{
	GameObject* parent = DBG_NEW GameObject;
	char rawname[100];
	strcpy(rawname, std::filesystem::path(path).stem().string().c_str());
	ComponentTransform* transf = DBG_NEW ComponentTransform(math::float4x4::identity); // put correct position of the saved obj
	GameObject* parentObj = DBG_NEW GameObject(transf, rawname, App->scene_intro->rootObj);
	LoadSceneNode(parentObj, path);

	return parent;
}
