#pragma once

#include "Glew/include/GL/glew.h"
#include "SDL\include\SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>

#pragma comment (lib, "Glew/lib/glew32.lib")    /* link OpenGL Utility lib     */
#pragma comment (lib, "glu32.lib")    /* link OpenGL Utility lib     */
#pragma comment (lib, "opengl32.lib") /* link Microsoft OpenGL lib   */

#include "SmileApp.h"
#include "SmileScene.h"
#include "SmileInput.h"
#include "SmileWindow.h"
#include "SmileFBX.h"

#include "MathGeoLib/include/Math/float2.h"
#include "MathGeoLib/include/Geometry/Ray.h"

#ifdef NDEBUG //no debug
#pragma comment (lib, "MathGeoLib/libx86/ReleaseLib/MathGeoLib.lib") 
#else
#pragma comment (lib, "MathGeoLib/libx86/DebugLib/MathGeoLib.lib") 
#endif

#include "ComponentMesh.h"

namespace rayTracer
{
	std::variant<ComponentMesh*, GameObject*> MouseOverMesh(int mouse_x, int mouse_y, bool assignClicked, bool GetMeshNotGameObject); // if you just want to assign the mesh as the selected in the scene, pass it true
	
}