#pragma once

// Warning disabled ---
#pragma warning( disable : 4577 ) // Warning that exceptions are disabled
#pragma warning( disable : 4530 )

#include <windows.h>
#include <stdio.h>

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifdef _DEBUG
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
// Replace _NORMAL_BLOCK with _CLIENT_BLOCK if you want the
// allocations to be of _CLIENT_BLOCK type
#else
#define DBG_NEW new
#endif

#define ReportMemoryLeaks() _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF)

#define LOG(format, ...) _log(__FILE__, __LINE__, format, __VA_ARGS__);

void _log(const char file[], int line, const char* format, ...);

#define CAP(n) ((n <= 0.0f) ? n=0.0f : (n >= 1.0f) ? n=1.0f : n=n)

#define DEGTORAD 0.0174532925199432957f
#define RADTODEG 57.295779513082320876f
#define _PI 3.141592653589793238462643383279502884197169399375105820974944f

typedef unsigned int uint;
typedef unsigned long long SmileUUID;

enum update_status
{
	UPDATE_CONTINUE = 1,
	UPDATE_STOP,
	UPDATE_ERROR
};

// Configuration -----------
#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 600
#define SCREEN_SIZE 1
#define WIN_FULLSCREEN false
#define WIN_RESIZABLE true
#define WIN_BORDERLESS false
#define WIN_FULLSCREEN_DESKTOP false
#define VSYNC true
#define TITLE "Smile 3D Engine"
#define ORGANIZATION "SmileTek"
#define LIBRARY_FOLDER "/Library/"
#define ASSETS_FOLDER "/Assets/"
#define LIBRARY_MESHES_FOLDER "/Library/Meshes/"
#define LIBRARY_TEXTURES_FOLDER "/Library/Textures/"
#define LIBRARY_MODELS_FOLDER "/Library/Models/"
#define ASSETS_MODELS_FOLDER "/Assets/Models/"
#define MESH_EXTENSION "smilemesh"
#define TEXTURES_EXTENSION "smiletexture"
#define MODELS_EXTENSION "smilemodel"

#include <string>
static bool IsModelExtension(std::string extension)
{ 
	if (extension == MODELS_EXTENSION || extension == "FBX" || extension == "fbx" || extension == "json")
		return true; 
	return false; 	
}

static bool IsOwnModelExtension(std::string extension) { 
	if (extension == MODELS_EXTENSION || extension == "json")
		return true; 
	return false; 
}

// Buffers -----------
// Deletes a buffer
#define RELEASE( x ) \
    {                        \
    if( x != NULL )        \
	    {                      \
      delete x;            \
	  x = NULL;              \
	    }                      \
    }

// Deletes an array of buffers
#define RELEASE_ARRAY( x ) \
    {                              \
    if( x != NULL )              \
	    {                            \
      delete[] x;                \
	  x = NULL;                    \
	    }                            \
                              \
    }


