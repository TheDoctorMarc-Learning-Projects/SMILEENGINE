#pragma once
class SmileApp;

class SmileModule
{
private:
	bool enabled = false;

public:
	SmileApp* App;

	SmileModule(SmileApp* parent, bool start_enabled = true) : App(parent)
	{}

	virtual ~SmileModule()
	{}

	virtual bool Init()
	{
		return true;
	}

	virtual bool Start()
	{
		return true;
	}

	virtual update_status PreUpdate(float dt)
	{
		return UPDATE_CONTINUE;
	}

	virtual update_status Update(float dt)
	{
		return UPDATE_CONTINUE;
	}

	virtual update_status PostUpdate(float dt)
	{
		return UPDATE_CONTINUE;
	}

	virtual bool CleanUp()
	{
		return true;
	}
};