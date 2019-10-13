#pragma once

#include "SmileModule.h"

class SmileGameObjectManager : public SmileModule
{
public:
	SmileGameObjectManager(SmileApp* app, bool start_enabled = true);
	~SmileGameObjectManager();

};