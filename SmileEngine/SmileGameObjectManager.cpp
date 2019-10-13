#include "SmileGameObjectManager.h"
#include "SmileSetup.h"
#include "SmileApp.h"
#include "ComponentTypes.h"

SmileGameObjectManager::SmileGameObjectManager(SmileApp* app, bool start_enabled) : SmileModule(app, start_enabled)
{
}
SmileGameObjectManager::~SmileGameObjectManager()
{
}

// -----------------------------------------------------------------
/*bool SmileGameObjectManager::Start()
{
	uniquenessMap =
	{
		{ COMPONENT_TYPE::TRANSFORM, std::tuple(true, true, false) },
		{ COMPONENT_TYPE::MESH,  std::tuple(true, false, true) },
		{ COMPONENT_TYPE::MATERIAL,  std::tuple(false, true, false) },
		{ COMPONENT_TYPE::LIGHT,  std::tuple(true, false, false) },
	};

	return true;
}
// -----------------------------------------------------------------
update_status SmileGameObjectManager::Update(float dt)
{
	return UPDATE_CONTINUE;
}

// -----------------------------------------------------------------
bool SmileGameObjectManager::CleanUp()
{
	const_cast<std::map<COMPONENT_TYPE, std::tuple<bool, bool, bool>>>(uniquenessMap).clear();
	return true;
}*/