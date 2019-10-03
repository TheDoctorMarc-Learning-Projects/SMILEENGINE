#define PAR_SHAPES_IMPLEMENTATION
#include "parshapes/par_shapes.h"

#include "SmileSetup.h"
#include "SmileApp.h"
#include "SmileScene.h"
/*#include <gl/GL.h>
//#include <gl/GLU.h>*/

#include "Glew/include/GL/glew.h"

#include "MathGeoLib/include/MathGeoLib.h"


#ifdef NDEBUG //no debug
#pragma comment (lib, "MathGeoLib/libx86/ReleaseLib/MathGeoLib.lib") 
#else
#pragma comment (lib, "MathGeoLib/libx86/DebugLib/MathGeoLib.lib") 
#endif


SmileScene::SmileScene(SmileApp* app, bool start_enabled) : SmileModule(app, start_enabled)
{
}

SmileScene::~SmileScene()
{}

// Load assets
bool SmileScene::Start()
{ 
	testCube = par_shapes_create_cube(); 

	// Vertex buffer
	glGenBuffers(1, &vertexID);
	glBindBuffer(GL_ARRAY_BUFFER, vertexID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * testCube->npoints * 3, &testCube->points[0], GL_STATIC_DRAW);

	// Index buffer
	glGenBuffers(1, &indexID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * testCube->ntriangles * 3, &testCube->triangles[0], GL_STATIC_DRAW);

	return true;
}


// Load assets
bool SmileScene::CleanUp()
{
	par_shapes_free_mesh(testCube); 

	return true;
}

// Update
update_status SmileScene::Update(float dt)
{
    
	DrawGrid();
 
	glColor3f(0.f, 0.f, 1.f);

	// Test: drawing a par_shapes cube 
	glEnableClientState(GL_VERTEX_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, vertexID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexID);
	glVertexPointer(3, GL_FLOAT, 0, NULL);
	glDrawElements(GL_TRIANGLES, testCube->ntriangles * 3, GL_UNSIGNED_SHORT, NULL);

	glDisableClientState(GL_VERTEX_ARRAY);
 
	return UPDATE_CONTINUE;
}

void SmileScene::DrawGrid()
{
	
	// TODO
}
