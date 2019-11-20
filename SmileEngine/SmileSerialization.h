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
	GameObject* LoadScene(const char* path);
private:

	
	bool SaveSceneNode(GameObject* go, rapidjson::Writer<rapidjson::StringBuffer>& writer);
	bool LoadSceneNode(GameObject* go, const char* path);
	

};