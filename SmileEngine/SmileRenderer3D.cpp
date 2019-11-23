#include "SmileSetup.h"
#include "SmileApp.h"
#include "SmileRenderer3D.h"
#include "SmileGui.h"

#include "Glew/include/GL/glew.h"
#include "SDL\include\SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>

#pragma comment (lib, "Glew/lib/glew32.lib")    /* link OpenGL Utility lib     */
#pragma comment (lib, "glu32.lib")    /* link OpenGL Utility lib     */
#pragma comment (lib, "opengl32.lib") /* link Microsoft OpenGL lib   */

#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_opengl3.h"

#include "ComponentCamera.h"
#include "GameObject.h"
#include "ComponentTransform.h"


SmileRenderer3D::SmileRenderer3D(SmileApp* app, bool start_enabled) : SmileModule(app, start_enabled)
{ 
}


// Destructor
SmileRenderer3D::~SmileRenderer3D()
{}

// Called before render is available
bool SmileRenderer3D::Init()
{
	LOG("Creating 3D Renderer context");
	bool ret = true;

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
	//Create contexts
	context = SDL_GL_CreateContext(App->window->window);
	if (context == NULL)
	{
		LOG("OpenGL context could not be created! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	GLenum error = glewInit();
	if (error != GL_NO_ERROR)
	{
		LOG("Error initializing glew library! %s\n", SDL_GetError());
		ret = false;
	}
	if (ret == true)
	{
		//Use Vsync
		if (VSYNC && SDL_GL_SetSwapInterval(1) < 0)
			LOG("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());

		//Initialize Projection Matrix
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		GLenum error = glGetError();
		if (error != GL_NO_ERROR)
		{
			LOG("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}

		//Initialize Modelview Matrix
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
		glClearDepth(1.0f);

		//Initialize clear color
		glClearColor(0.f, 0.f, 0.f, 1.f);

		GLfloat LightModelAmbient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, LightModelAmbient);

		lights[0].ref = GL_LIGHT0;
		lights[0].ambient.Set(0.25f, 0.25f, 0.25f, 1.0f);
		lights[0].diffuse.Set(0.75f, 0.75f, 0.75f, 1.0f);
		lights[0].SetPos(0.0f, 0.0f, 2.5f);
		lights[0].Init();

		GLfloat MaterialAmbient[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, MaterialAmbient);

		GLfloat MaterialDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, MaterialDiffuse);

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		lights[0].Active(true);
		glEnable(GL_LIGHTING);
		glEnable(GL_COLOR_MATERIAL);


	}

	return ret;
}

bool SmileRenderer3D::Start()
{
	// Projection matrix 
	OnResize(std::get<int>(App->window->GetWindowParameter("Width")), std::get<int>(App->window->GetWindowParameter("Height")),
		App->scene_intro->debugCamera);
	
	return true; 
}

// PreUpdate: clear buffer
update_status SmileRenderer3D::PreUpdate(float dt)
{
	vec3 camPos = targetCamera->GetParent()->GetTransform()->GetPositionVec3();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(targetCamera->GetViewMatrix());

	// light 0 on cam pos
	lights[0].SetPos(camPos.x, camPos.y, camPos.z);

	for (uint i = 0; i < MAX_LIGHTS; ++i)
		lights[i].Render();

	return UPDATE_CONTINUE;
}

// PostUpdate present buffer to screen
update_status SmileRenderer3D::PostUpdate(float dt)
{

    App->gui->HandleRender(); 
	SDL_GL_SwapWindow(App->window->window);

	return UPDATE_CONTINUE;
}

bool SmileRenderer3D::Reset()
{
	targetCamera = nullptr; 
	return true; 
}

// Called before quitting
bool SmileRenderer3D::CleanUp()
{
	LOG("Destroying 3D Renderer");

	SDL_GL_DeleteContext(context);

	return true;
}


void SmileRenderer3D::OnResize(int width, int height, ComponentCamera* targetCam)
{ 	
	if(targetCam != targetCamera)
		targetCamera = targetCam;


	// compute spatial data
	targetCamera->_renderingData.ratio = (float)width / (float)height;
	targetCamera->ComputeSpatialData();
	targetCamera->GetFrustrum()->CalculatePlanes();

	renderingData data = targetCamera->GetRenderingData();

	glViewport(0, 4, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	ProjectionMatrix = perspective(data.fovYangle, data.ratio,
		data.pNearDist, data.pFarDist);
	glLoadMatrixf(&ProjectionMatrix);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

}

float* SmileRenderer3D::GetProjectionMatrix()
{
	return &ProjectionMatrix; 
}


float* SmileRenderer3D::GetProjectionMatrixTransposed()
{
	mat4x4 ret = ProjectionMatrix;
	ret.transpose();
	return &ret;
}

mat4x4 SmileRenderer3D::GetProjectionMatrixTransposedA()
{
	mat4x4 ret = ProjectionMatrix;
	ret.transpose();
	return ret;
}


void SmileRenderer3D::SwitchCamera()
{
	ComponentCamera* gameCam = App->scene_intro->gameCamera;
	ComponentCamera* debugCam = App->scene_intro->debugCamera;

	if (targetCamera == gameCam)
		targetCamera = debugCam;
	else if (targetCamera == debugCam)
		targetCamera = gameCam;

	OnResize(std::get<int>(App->window->GetWindowParameter("Width")), std::get<int>(App->window->GetWindowParameter("Height")),
		targetCamera);

}

void SmileRenderer3D::SetTargetCamera(ComponentCamera* cam)
{
	targetCamera = cam; 
	OnResize(std::get<int>(App->window->GetWindowParameter("Width")), std::get<int>(App->window->GetWindowParameter("Height")),
		targetCamera);
}