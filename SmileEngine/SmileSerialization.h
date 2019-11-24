#pragma once

#include "SmileModule.h"
#include "SmileSetup.h"
#include"JSONParser.h"

class GameObject;

class SmileSerialization : public SmileModule
{
public:
	SmileSerialization(SmileApp* app, bool start_enabled = true);
	~SmileSerialization();
	bool SaveScene();
	void LoadScene(const char* path, bool startup = false);
private:

	
	bool SaveSceneNode(GameObject* go, rapidjson::Writer<rapidjson::StringBuffer>& writer);
	GameObject* LoadSceneNode(GameObject* go, rapidjson::Value& mynode, rapidjson::Document& doc);
	

};