#include "SmileSetup.h"
#include "SmileApp.h"
#include "SmileCamera3D.h"
#include "QuickMath.h"
#include "RayTracer.h"

SmileCamera3D::SmileCamera3D(SmileApp* app, bool start_enabled) : SmileModule(app, start_enabled)
{
	CalculateViewMatrix();

	X = vec3(1.0f, 0.0f, 0.0f);
	Y = vec3(0.0f, 1.0f, 0.0f);
	Z = vec3(0.0f, 0.0f, 1.0f);

	Position = vec3(0.0f, 0.0f, 5.0f);
	Reference = vec3(0.0f, 0.0f, 0.0f);
}

SmileCamera3D::~SmileCamera3D()
{}

// -----------------------------------------------------------------
bool SmileCamera3D::Start()
{
	LOG("Setting up the camera");
	bool ret = true;

	return ret;
}

// -----------------------------------------------------------------
bool SmileCamera3D::CleanUp()
{
	LOG("Cleaning camera");

	return true;
}

// ----------------------------------------------------------------- 
// This must be done before clearing the Depth buffer in the Render PreUpdate
update_status SmileCamera3D::PreUpdate(float dt)
{
	// Check if the user clicks to select object 
	if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN)
		rayTracer::AssignSelectedMeshOnMouseClick(App->input->GetMouseX(), App->input->GetMouseY());

	return UPDATE_CONTINUE;
}

// -----------------------------------------------------------------
update_status SmileCamera3D::Update(float dt)
{
	// Focus an object ----------------
	if (App->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN)
		if (App->scene_intro->selected_mesh != nullptr)
			FitMeshToCamera(App->scene_intro->selected_mesh);

	vec3 newPos(0, 0, 0);
	float speed = 10.0f * dt;

	// FPS-Like free movement  ----------------
	if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)
		speed *= 2; 

	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) newPos -= Z * speed;
	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) newPos += Z * speed;


	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) newPos -= X * speed;
	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) newPos += X * speed;


	// Zoom, Pan ----------------
	int zScroll = -App->input->GetMouseZ();
	int xWheel = -App->input->GetMouseXMotion(); 
	int yWheel = App->input->GetMouseYMotion();
	KEY_STATE mouseMiddle = App->input->GetMouseButton(SDL_BUTTON_MIDDLE);
	 
	if (xWheel != 0 && (mouseMiddle == KEY_DOWN || mouseMiddle == KEY_REPEAT))
		newPos += X * speed * quickMath::Sign(xWheel);

	if (yWheel != 0 && (mouseMiddle == KEY_DOWN || mouseMiddle == KEY_REPEAT))
		newPos += Y * speed * quickMath::Sign(yWheel);

	if (zScroll != 0)
		newPos += Z * speed * quickMath::Sign(zScroll);

	Position += newPos;
	Reference += newPos;

	// Rotation ----------------
	if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT)
	{
		float Sensitivity = 0.25f;

		Position -= Reference;

		if (xWheel != 0)
		{
			float DeltaX = (float)xWheel * Sensitivity;

			X = rotate(X, DeltaX, vec3(0.0f, 1.0f, 0.0f));
			Y = rotate(Y, DeltaX, vec3(0.0f, 1.0f, 0.0f));
			Z = rotate(Z, DeltaX, vec3(0.0f, 1.0f, 0.0f));
		}

		if (yWheel != 0)
		{
			float DeltaY = -(float)yWheel * Sensitivity;

			Y = rotate(Y, DeltaY, X);
			Z = rotate(Z, DeltaY, X);

			if (Y.y < 0.0f)
			{
				Z = vec3(0.0f, Z.y > 0.0f ? 1.0f : -1.0f, 0.0f);
				Y = cross(Z, X);
			}
		}

		Position = Reference + Z * length(Position);
	}

	// Recalculate matrix -------------
	CalculateViewMatrix();

	return UPDATE_CONTINUE;
}

// -----------------------------------------------------------------
void SmileCamera3D::Look(const vec3& Position, const vec3& Reference, bool RotateAroundReference)
{
	this->Position = Position;
	this->Reference = Reference;

	Z = normalize(Position - Reference);
	X = normalize(cross(vec3(0.0f, 1.0f, 0.0f), Z));
	Y = cross(Z, X);

	if (!RotateAroundReference)
	{
		this->Reference = this->Position;
		this->Position += Z * 0.05f;
	}

	CalculateViewMatrix();
}

// -----------------------------------------------------------------
void SmileCamera3D::LookAt(const vec3& Spot)
{
	Reference = Spot;

	Z = normalize(Position - Reference);
	X = normalize(cross(vec3(0.0f, 1.0f, 0.0f), Z));
	Y = cross(Z, X);

	CalculateViewMatrix();
}


// -----------------------------------------------------------------
void SmileCamera3D::Move(const vec3& Movement)
{
	Position += Movement;
	Reference += Movement;

	CalculateViewMatrix();
}

// -----------------------------------------------------------------
float* SmileCamera3D::GetViewMatrix()
{
	return &ViewMatrix;
}


// -----------------------------------------------------------------
float* SmileCamera3D::GetViewMatrixInverse()
{
	return &ViewMatrixInverse;
}

// -----------------------------------------------------------------
void SmileCamera3D::CalculateViewMatrix()
{
	ViewMatrix = mat4x4(X.x, Y.x, Z.x, 0.0f, X.y, Y.y, Z.y, 0.0f, X.z, Y.z, Z.z, 0.0f, -dot(X, Position), -dot(Y, Position), -dot(Z, Position), 1.0f);
	ViewMatrixInverse = inverse(ViewMatrix);
}

// -----------------------------------------------------------------
void SmileCamera3D::FitMeshToCamera(Mesh* mesh)
{
	// Compute the distance the camera should be to fit the entire bounding sphere
	LookAt(mesh->GetMeshCenter());

	double camDistance = (mesh->GetMeshSphereRadius()) / math::Tan(math::DegToRad(FOV_Y / 2.F));
	math::float3 dir = (math::float3(Position.x, Position.y, Position.z) - math::float3(mesh->GetMeshCenter().x, mesh->GetMeshCenter().y, mesh->GetMeshCenter().z)); 
	dir.Normalize();
	vec3 dirVec3(dir.x, dir.y, dir.z);
	vec3 wantedPos = mesh->GetMeshCenter() + dirVec3 * camDistance;
	
	Move(vec3(wantedPos - Position));
}
 

	 
