#include "SmileSetup.h"
#include "SmileApp.h"
#include "SmileFileSystem.h"
//#include "ModuleResources.h"
#include "../SmileEngine/PhysFS/include/physfs.h"
#include "Assimp/include/cfileio.h"
#include "Assimp/include/types.h"


#pragma comment( lib, "PhysFS/libx86/physfs.lib" )

//#include "../SmileEngine/mmgr/mmgr.h"

using namespace std;

SmileFileSystem::SmileFileSystem(SmileApp* app, bool start_enabled, const char* game_path) : SmileModule(app, true) 
{
	// needs to be created before Init so other modules can use it
	char* base_path = SDL_GetBasePath();
	PHYSFS_init(base_path);
	SDL_free(base_path);

	// workaround VS string directory mess
	AddPath(".");

	if (0 && game_path != nullptr)
		AddPath(game_path);

	// Dump list of paths
	LOG("FileSystem Operations base is [%s] plus:", GetBasePath());
	LOG(GetReadPaths());

	// enable us to write in the game's dir area
	if (PHYSFS_setWriteDir(".") == 0)
		LOG("File System error while creating write dir: %s\n", PHYSFS_getLastError());

	// Make sure standard paths exist
	const char* dirs[] = {
		/* ASSETS_FOLDER,*/ LIBRARY_FOLDER,

	};

	for (uint i = 0; i < sizeof(dirs) / sizeof(const char*); ++i)
	{
		if (PHYSFS_exists(dirs[i]) == 0)
			PHYSFS_mkdir(dirs[i]);
	}

	// Generate IO interfaces
	CreateAssimpIO();
	//CreateBassIO();
}

// Destructor
SmileFileSystem::~SmileFileSystem()
{
	RELEASE(AssimpIO);
	//RELEASE(BassIO);
	PHYSFS_deinit();
}

// Called before render is available
bool SmileFileSystem::Start()
{
	LOG("Loading File System");
	bool ret = true;

	// Ask SDL for a write dir
	char* write_path = SDL_GetPrefPath(ORGANIZATION, TITLE);

	// Trun this on while in game mode
	//if(PHYSFS_setWriteDir(write_path) == 0)
		//LOG("File System error while creating write dir: %s\n", PHYSFS_getLastError());


	SDL_free(write_path);

	return ret;
}

// Called before quitting
bool SmileFileSystem::CleanUp()
{
	//LOG("Freeing File System subsystem");

	return true;
}

// Add a new zip file or folder
bool SmileFileSystem::AddPath(const char* path_or_zip)
{
	bool ret = false;

	if (PHYSFS_mount(path_or_zip, nullptr, 1) == 0) {
		LOG("File System error while adding a path or zip: %s\n", PHYSFS_getLastError());
	}
	else
		ret = true;

	return ret;
}

// Check if a file exists
bool SmileFileSystem::Exists(const char* file) const
{
	return PHYSFS_exists(file) != 0;
}

// Check if a file is a directory
bool SmileFileSystem::IsDirectory(const char* file) const
{
	return PHYSFS_isDirectory(file) != 0;
}

void SmileFileSystem::CreateDirectory(const char* directory)
{
	PHYSFS_mkdir(directory);
}

void SmileFileSystem::DiscoverFiles(const char* directory, vector<string>& file_list, vector<string>& dir_list) const
{
	char** rc = PHYSFS_enumerateFiles(directory);
	char** i;

	string dir(directory);

	for (i = rc; *i != nullptr; i++)
	{
		if (PHYSFS_isDirectory((dir + *i).c_str()))
			dir_list.push_back(*i);
		else
			file_list.push_back(*i);
	}

	PHYSFS_freeList(rc);
}

bool SmileFileSystem::CopyFromOutsideFS(const char* full_path, const char* destination)
{
	// Only place we acces non virtual filesystem
	bool ret = false;

	char buf[8192];
	size_t size;

	FILE* source = nullptr;
	fopen_s(&source, full_path, "rb");
	PHYSFS_file* dest = PHYSFS_openWrite(destination);

	if (source && dest)
	{
		while (size = fread_s(buf, 8192, 1, 8192, source))
			PHYSFS_write(dest, buf, 1, size);

		fclose(source);
		PHYSFS_close(dest);
		ret = true;

		LOG("File System copied file [%s] to [%s]", full_path, destination);
	}
	else
		LOG("File System error while copy from [%s] to [%s]", full_path, destination);

	return ret;
}

bool SmileFileSystem::Copy(const char* source, const char* destination)
{
	bool ret = false;

	char buf[8192];

	PHYSFS_file* src = PHYSFS_openRead(source);
	PHYSFS_file* dst = PHYSFS_openWrite(destination);

	PHYSFS_sint32 size;
	if (src && dst)
	{
		while (size = (PHYSFS_sint32)PHYSFS_read(src, buf, 1, 8192))
			PHYSFS_write(dst, buf, 1, size);

		PHYSFS_close(src);
		PHYSFS_close(dst);
		ret = true;

		LOG("File System copied file [%s] to [%s]", source, destination);
	}
	else
		LOG("File System error while copy from [%s] to [%s]", source, destination);

	return ret;
}

void SmileFileSystem::SplitFilePath(const char* full_path, std::string* path, std::string* file, std::string* extension) const
{
	if (full_path != nullptr)
	{
		string full(full_path);
		NormalizePath(full);
		size_t pos_separator = full.find_last_of("\\/");
		size_t pos_dot = full.find_last_of(".");

		if (path != nullptr)
		{
			if (pos_separator < full.length())
				*path = full.substr(0, pos_separator + 1);
			else
				path->clear();
		}

		if (file != nullptr)
		{
			if (pos_separator < full.length())
				*file = full.substr(pos_separator + 1);
			else
				*file = full;
		}

		if (extension != nullptr)
		{
			if (pos_dot < full.length())
				*extension = full.substr(pos_dot + 1);
			else
				extension->clear();
		}
	}
}

// Flatten filenames to always use lowercase and / as folder separator
char normalize_char(char c)
{
	if (c == '\\')
		return '/';
	return tolower(c);
}

void SmileFileSystem::NormalizePath(char* full_path) const
{
	int len = strlen(full_path);
	for (int i = 0; i < len; ++i)
	{
		if (full_path[i] == '\\')
			full_path[i] = '/';
		else
			full_path[i] = tolower(full_path[i]);
	}
}

void SmileFileSystem::NormalizePath(std::string& full_path) const
{
	for (string::iterator it = full_path.begin(); it != full_path.end(); ++it)
	{
		if (*it == '\\')
			*it = '/';
		else
			*it = tolower(*it);
	}
}



unsigned int SmileFileSystem::Load(const char* path, const char* file, char** buffer) const
{
	string full_path(path);
	full_path += file;
	return Load(full_path.c_str(), buffer);
}

// Read a whole file and put it in a new buffer
uint SmileFileSystem::Load(const char* file, char** buffer) const
{
	uint ret = 0;

	PHYSFS_file* fs_file = PHYSFS_openRead(file);

	if (fs_file != nullptr)
	{
		PHYSFS_sint32 size = (PHYSFS_sint32)PHYSFS_fileLength(fs_file);

		if (size > 0)
		{
			*buffer = new char[size];
			uint readed = (uint)PHYSFS_read(fs_file, *buffer, 1, size);
			if (readed != size)
			{
				LOG("File System error while reading from file %s: %s\n", file, PHYSFS_getLastError());
				RELEASE_ARRAY(buffer);
			}
			else
				ret = readed;
		}

		if (PHYSFS_close(fs_file) == 0)
			LOG("File System error while closing file %s: %s\n", file, PHYSFS_getLastError());
	}
	else
		LOG("File System error while opening file %s: %s\n", file, PHYSFS_getLastError());

	return ret;
}

// Read a whole file and put it in a new buffer
SDL_RWops* SmileFileSystem::Load(const char* file) const
{
	char* buffer;
	int size = Load(file, &buffer);

	if (size > 0)
	{
		SDL_RWops* r = SDL_RWFromConstMem(buffer, size);
		if (r != nullptr)
			r->close = close_sdl_rwops;

		return r;
	}
	else
		return nullptr;
}

uint SmileFileSystem::ReadFile(const char* file_name, char** buffer)
{
	PHYSFS_File* file = PHYSFS_openRead(file_name);
	uint size = 0u;

	if (file != nullptr)
	{
		size = PHYSFS_fileLength(file);

		*buffer = new char[size];

		if (PHYSFS_read(file, *buffer, 1, size) == -1)
		{
			size = 0u;
			LOG("GL");
		}
		else
			LOG("DONE");
	}
	PHYSFS_close(file);
	return size;
}

//void * ModuleFileSystem::BassLoad(const char * file) const
//{
//	PHYSFS_file* fs_file = PHYSFS_openRead(file);
//
//	if (fs_file == nullptr)
//		LOG("File System error while opening file %s: %s\n", file, PHYSFS_getLastError());
//
//	return (void*)fs_file;
//}

int close_sdl_rwops(SDL_RWops* rw)
{
	RELEASE_ARRAY(rw->hidden.mem.base);
	SDL_FreeRW(rw);
	return 0;
}

bool SmileFileSystem::WriteRaw(const char* file, const void* buffer, unsigned int size) const
{
	bool ret = false; 

	PHYSFS_file* fs_file = PHYSFS_openWrite(file);

	if (file && PHYSFS_writeBytes(fs_file, (const void*)buffer, size) >= size)
	{
		ret = true;
		LOG("FILESYSTEM: success on writting in file [ %s ]", file);
	}
	else
		LOG("FILESYSTEM: failure on writting in file [ %s ]: %s", file, PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));

	if (PHYSFS_close(fs_file) == 0)
		LOG("File System error while closing file %s: %s", file, PHYSFS_getLastError());

	return ret; 
}

// Save a whole buffer to disk
uint SmileFileSystem::Save(const char* file, const void* buffer, unsigned int size, bool append) const
{
	unsigned int ret = 0;

	bool overwrite = PHYSFS_exists(file) != 0;
	PHYSFS_file* fs_file = (append) ? PHYSFS_openAppend(file) : PHYSFS_openWrite(file);

	if (fs_file != nullptr)
	{
		uint written = (uint)PHYSFS_write(fs_file, (const void*)buffer, 1, size);
		if (written != size) {
			LOG("File System error while writing to file %s: %s", file, PHYSFS_getLastError());
		}
		else
		{
			if (append == true) {
				LOG("Added %u data to [%s%s]", size, PHYSFS_getWriteDir(), file);
				//else if(overwrite == true)
					//LOG("File [%s%s] overwritten with %u bytes", PHYSFS_getWriteDir(), file, size);
			}
			else if (overwrite == false)
				LOG("New file created [%s%s] of %u bytes", PHYSFS_getWriteDir(), file, size);

			ret = written;
		}

		if (PHYSFS_close(fs_file) == 0)
			LOG("File System error while closing file %s: %s", file, PHYSFS_getLastError());
	}
	else
		LOG("File System error while opening file %s: %s", file, PHYSFS_getLastError());

	return ret;
}

bool SmileFileSystem::SaveUnique(string& name, const void* buffer, uint size, const char* path, const char* prefix, const char* extension)
{
	char result[250];

	//sprintf_s(result, 250, "%s%s_%llu.%s", path, prefix, (unsigned long long)1 , extension); //App->resources->GenerateNewUID() when we have UIDs
	sprintf_s(result, 250, "%s%s.%s", path, prefix, extension);
	NormalizePath(result);
	if (!Exists(path)) {
		CreateDirectory(path);
		LOG("Creating directory %s", path);
	}

	if (Save(result, buffer, size) > 0)
	{
		name = result;
		return true;
	}
	return false;
}

bool SmileFileSystem::Remove(const char* file)
{
	bool ret = false;

	if (file != nullptr)
	{
		if (PHYSFS_delete(file) == 0)
		{
			LOG("File deleted: [%s]", file);
			ret = true;
		}
		else
			LOG("File System error while trying to delete [%s]: ", file, PHYSFS_getLastError());
	}

	return ret;
}

const char* SmileFileSystem::GetBasePath() const
{
	return PHYSFS_getBaseDir();
}

const char* SmileFileSystem::GetWritePath() const
{
	return PHYSFS_getWriteDir();
}

const char* SmileFileSystem::GetReadPaths() const
{
	static char paths[512];

	paths[0] = '\0';

	char** path;
	for (path = PHYSFS_getSearchPath(); *path != nullptr; path++)
	{
		strcat_s(paths, 512, *path);
		strcat_s(paths, 512, "\n");
	}

	return paths;
}


std::string SmileFileSystem::GetDirectoryFromPath(const char* file)	  
{
	std::string path = file;

	uint unwanted = path.find_last_of("\\/"); 

	if (unwanted != std::string::npos)
		path = path.erase(unwanted + 1);

	return path;
}


// -----------------------------------------------------
// ASSIMP IO
// -----------------------------------------------------

size_t AssimpWrite(aiFile* file, const char* data, size_t size, size_t chunks)
{
	PHYSFS_sint64 ret = PHYSFS_write((PHYSFS_File*)file->UserData, (void*)data, size, chunks);
	if (ret == -1)
		LOG("File System error while WRITE via assimp: %s", PHYSFS_getLastError());

	return (size_t)ret;
}

size_t AssimpRead(aiFile* file, char* data, size_t size, size_t chunks)
{
	PHYSFS_sint64 ret = PHYSFS_read((PHYSFS_File*)file->UserData, (void*)data, size, chunks);
	if (ret == -1)
		LOG("File System error while READ via assimp: %s", PHYSFS_getLastError());

	return (size_t)ret;
}

size_t AssimpTell(aiFile* file)
{
	PHYSFS_sint64 ret = PHYSFS_tell((PHYSFS_File*)file->UserData);
	if (ret == -1)
		LOG("File System error while TELL via assimp: %s", PHYSFS_getLastError());

	return (size_t)ret;
}

size_t AssimpSize(aiFile* file)
{
	PHYSFS_sint64 ret = PHYSFS_fileLength((PHYSFS_File*)file->UserData);
	if (ret == -1)
		LOG("File System error while SIZE via assimp: %s", PHYSFS_getLastError());

	return (size_t)ret;
}

void AssimpFlush(aiFile* file)
{
	if (PHYSFS_flush((PHYSFS_File*)file->UserData) == 0)
		LOG("File System error while FLUSH via assimp: %s", PHYSFS_getLastError());
}

aiReturn AssimpSeek(aiFile* file, size_t pos, aiOrigin from)
{
	int res = 0;

	switch (from)
	{
	case aiOrigin_SET:
		res = PHYSFS_seek((PHYSFS_File*)file->UserData, pos);
		break;
	case aiOrigin_CUR:
		res = PHYSFS_seek((PHYSFS_File*)file->UserData, PHYSFS_tell((PHYSFS_File*)file->UserData) + pos);
		break;
	case aiOrigin_END:
		res = PHYSFS_seek((PHYSFS_File*)file->UserData, PHYSFS_fileLength((PHYSFS_File*)file->UserData) + pos);
		break;
	}

	if (res == 0)
		LOG("File System error while SEEK via assimp: %s", PHYSFS_getLastError());

	return (res != 0) ? aiReturn_SUCCESS : aiReturn_FAILURE;
}

aiFile* AssimpOpen(aiFileIO* io, const char* name, const char* format)
{
	static aiFile file;

	file.UserData = (char*)PHYSFS_openRead(name);
	file.ReadProc = AssimpRead;
	file.WriteProc = AssimpWrite;
	file.TellProc = AssimpTell;
	file.FileSizeProc = AssimpSize;
	file.FlushProc = AssimpFlush;
	file.SeekProc = AssimpSeek;

	return &file;
}

void AssimpClose(aiFileIO* io, aiFile* file)
{
	if (PHYSFS_close((PHYSFS_File*)file->UserData) == 0)
		LOG("File System error while CLOSE via assimp: %s", PHYSFS_getLastError());
}

void SmileFileSystem::CreateAssimpIO()
{
	RELEASE(AssimpIO);

	AssimpIO = new aiFileIO;
	AssimpIO->OpenProc = AssimpOpen;
	AssimpIO->CloseProc = AssimpClose;
}

aiFileIO* SmileFileSystem::GetAssimpIO()
{
	return AssimpIO;
}