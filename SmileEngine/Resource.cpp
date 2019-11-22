#include "SmileUtilitiesModule.h"
#include "Utility.h"
#include "RNG.h"
#include "SmileApp.h"
#include "Resource.h"

Resource::Resource(SmileUUID id, Resource_Type type)
{
	if (id == 0) // but what if... haha
		uid = dynamic_cast<RNG*>(App->utilities->GetUtility("RNG"))->GetRandomUUID();
	else
		uid = id; 
}
