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
	writer.StartObject();
	writer.Key("Mesh");
	
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
			writer.String("Empty");

		writer.Key("Selected");
		writer.Bool((App->scene_intro->selected_mesh == mesh) ? true : false);

		writer.EndObject();

	}
	else
		writer.String("empty"); 

	writer.EndObject();
	// - - - - - - - - - - - - (end mesh)


	writer.EndArray();
	// - - - - - - - - - - - - - - - - - - - - (end Components)

    // - - - - - - - - - - - - - - - - - - - - Children
	writer.Key("Children");
	if (children.size() > 0) {


		writer.StartArray();
		//writer.StartObject();
		for (auto& child : children)
		{
			SaveSceneNode(child, writer);
		}
		//writer.EndObject();
		writer.EndArray();

	}
	else
		writer.String("Empty"); 


	writer.EndObject();


	writer.EndObject(); // end gameObject object

	return false;
}

GameObject* SmileSerialization::LoadScene(const char* path)
{
	GameObject* parent = DBG_NEW GameObject;
	rapidjson::Document doc;
	dynamic_cast<JSONParser*>(App->utilities->GetUtility("JSONParser"))->ParseJSONFile(path, doc);
	int id = rapidjson::GetValueByPointer(doc, "/GameObject/0/UID")->GetInt();
	int parent_id = rapidjson::GetValueByPointer(doc, "/GameObject/0/Parent ID")->GetInt();

	std::string name = rapidjson::GetValueByPointer(doc, "/GameObject/0/Name")->GetString();
	bool selected = rapidjson::GetValueByPointer(doc, "/GameObject/0/Selected")->GetBool();

	char rawname[100];
	strcpy(rawname, std::filesystem::path(path).stem().string().c_str());
	ComponentTransform* transf = DBG_NEW ComponentTransform(math::float4x4::identity); // put correct position of the saved obj
	GameObject* parentObj = DBG_NEW GameObject(transf, rawname, App->scene_intro->rootObj);

	return parent;
}
