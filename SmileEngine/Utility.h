#pragma once

class Utility 
{
public: 
	virtual bool Initialize() { return true;  };
	virtual bool ShutDown() { return true;  };
};
