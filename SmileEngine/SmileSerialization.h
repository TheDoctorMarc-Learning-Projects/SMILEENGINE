#pragma once

#include "SmileModule.h"
#include "SmileSetup.h"


class SmileSerialization : public SmileModule
{
public:
	SmileSerialization(SmileApp* app, bool start_enabled = true);
	~SmileSerialization();

private:
	bool SaveScene();
	bool LoadScene();

};