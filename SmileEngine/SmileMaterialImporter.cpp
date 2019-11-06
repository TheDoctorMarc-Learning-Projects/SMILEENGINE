#include "SmileMaterialImporter.h"
#include "SmileApp.h"
#include "SmileSetup.h"

#pragma comment (lib, "Assimp/libx86/assimp.lib")
#pragma comment (lib, "DevIL/libx86/DevIL.lib")
#pragma comment (lib, "DevIL/libx86/ILU.lib")
#pragma comment (lib, "DevIL/libx86/ILUT.lib")

#include "DevIL/include/IL/il.h"
#include "DevIL/include/IL/ilu.h"
#include "DevIL/include/IL/ilut.h"


SmileMaterialImporter::SmileMaterialImporter(SmileApp* app, bool start_enabled) : SmileModule(app, start_enabled)
{

}



bool SmileMaterialImporter::Import(const char* file, const char* path, std::string& output_file)
{
	bool ret = false;
	ILuint size;
	ILubyte* data;
	ilSetInteger(IL_DXTC_FORMAT, IL_DXT5);// To pick a specific DXT compression use
	size = ilSaveL(IL_DDS, NULL, 0); // Get the size of the data buffer
	if (size > 0) {
		data = new ILubyte[size]; // allocate data buffer
		if (ilSaveL(IL_DDS, data, size) > 0) // Save to buffer with the ilSaveIL function
			ret = App->fs->SaveUnique(output_file, data, size, LIBRARY_FOLDER, "texture", "dds");
		RELEASE_ARRAY(data);
		return ret;
	}
}

bool SmileMaterialImporter::Import(const void* buffer, uint size, std::string& output_file)
{
	return false;
}

bool SmileMaterialImporter::Load(const char* exported_file, textureData* resource)
{
	return false;
}

bool SmileMaterialImporter::LoadCheckers(textureData* resource)
{
	return false;
}
