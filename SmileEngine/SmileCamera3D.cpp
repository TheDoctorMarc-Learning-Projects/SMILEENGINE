#include "SmileSetup.h"
//#include "SmileApp.h" // already included by raytracer 
//#include "SmileCamera3D.h" // already included by raytracer 
#include "QuickMath.h"
#include "RayTracer.h"

#include "ComponentTransform.h"

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
		rayTracer::MouseOverMesh(App->input->GetMouseX(), App->input->GetMouseY(), true, true);

	return UPDATE_CONTINUE;
}

// -----------------------------------------------------------------
update_status SmileCamera3D::Update(float dt)
{
	if (App->gui->IsGuiItemActive() == false)
	{
		// Focus an object ----------------
		FocusObjectLogic(); 

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
			newPos += Z * GetScrollSpeed(dt, zScroll);

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
	}
	

	// Recalculate matrix -------------
	CalculateViewMatrix();

	return UPDATE_CONTINUE;
}

void SmileCamera3D::FocusObjectLogic()
{
	ComponentMesh* selectedMesh = App->scene_intro->selected_mesh;
	GameObject* selectedObj = App->scene_intro->selectedObj;

	if (selectedMesh != nullptr)
	{
		if (App->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN)
			FitCameraToMesh(selectedMesh);

		LookAt(dynamic_cast<ComponentTransform*>(std::get<Component*>(selectedMesh->GetComponent(TRANSFORM)))->GetPosition());
	}
	else if (selectedObj != nullptr)
	{
		if (App->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN)
			FitCameraToObject(selectedObj);

		LookAt(dynamic_cast<ComponentTransform*>(std::get<Component*>(selectedObj->GetComponent(TRANSFORM)))->GetPosition());
	}
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
void SmileCamera3D::LookAt(const float3& Spot)
{
	vec3 SpotV(Spot.x, Spot.y, Spot.z); 

	Reference = SpotV;

	Z = normalize(Position - Reference);
	X = normalize(cross(vec3(0.0f, 1.0f, 0.0f), Z));
	Y = cross(Z, X);

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
void SmileCamera3D::FitCameraToMesh(ComponentMesh* mesh)
{

	// look at the mesh center
	float3 centerMath = dynamic_cast<ComponentTransform*>(std::get<Component*>(mesh->GetComponent(TRANSFORM)))->GetPosition(); 
	vec3 center(centerMath.x, centerMath.y, centerMath.z); 

	// we will need the bounding sphere radius
	ModelMeshData* data = mesh->GetMeshData(); 
	if (!data) 
		return; 
	
	LookAt(center);

	// calculate the distance with the center
	double camDistance = (data->GetMeshSphereRadius()) / math::Tan(math::DegToRad(FOV_Y / 2.F));
	math::float3 dir = (math::float3(Position.x, Position.y, Position.z) - math::float3(center.x, center.y, center.z));
	dir.Normalize();
	vec3 dirVec3(dir.x, dir.y, dir.z);

	// the targed pos (of the camera from the mesh) is the center plus the direction by the distance.
	vec3 wantedPos = center + dirVec3 * camDistance;

	// finally cap it 
	if (abs((wantedPos - center).z) < MIN_DIST_TO_MESH)
		wantedPos.z = MIN_DIST_TO_MESH;

	Move(vec3(wantedPos - Position));

	

}

// -----------------------------------------------------------------
void SmileCamera3D::FitCameraToObject(GameObject* obj) // TODO: calculate a radius for the bounding sphere of an object (the avg of the meshes centers)
{
	// look at the mesh center
	/*float3 centerMath = dynamic_cast<ComponentTransform*>(std::get<Component*>(obj->GetComponent(TRANSFORM)))->GetPosition();
	vec3 center(centerMath.x, centerMath.y, centerMath.z);

	// we will need the bounding sphere radius
	ModelMeshData* data =
	if (!data)
		return;

	LookAt(center);

	// calculate the distance with the center
	double camDistance = (data->GetMeshSphereRadius()) / math::Tan(math::DegToRad(FOV_Y / 2.F));
	math::float3 dir = (math::float3(Position.x, Position.y, Position.z) - math::float3(center.x, center.y, center.z));
	dir.Normalize();
	vec3 dirVec3(dir.x, dir.y, dir.z);

	// the targed pos (of the camera from the mesh) is the center plus the direction by the distance.
	vec3 wantedPos = center + dirVec3 * camDistance;

	// finally cap it 
	if (abs((wantedPos - center).z) < MIN_DIST_TO_MESH)
		wantedPos.z = MIN_DIST_TO_MESH;

	Move(vec3(wantedPos - Position));*/

}
 
float SmileCamera3D::GetScrollSpeed(float dt, float zScroll)
{
	float speed = DEFAULT_SPEED * dt * quickMath::Sign(zScroll);

	ComponentMesh* mesh = App->scene_intro->selected_mesh; 
	if (mesh != nullptr)
	{
		// some stupid conversions from vec3 to float3
		float3 captureCamPos(Position.x, Position.y, Position.z);
		float3 captureZ(Z.x, Z.y, Z.z); 

		float relSpeed = pow((abs((captureCamPos - dynamic_cast<ComponentTransform*>(std::get<Component*>(mesh->GetComponent(TRANSFORM)))->GetPosition()).Length())), EXPONENTIAL_ZOOM_FACTOR) * dt * quickMath::Sign(zScroll);
		relSpeed = (relSpeed > MAX_FRAME_SPEED) ? MAX_FRAME_SPEED : relSpeed;

		float targetZ = abs((captureCamPos + captureZ * relSpeed).z);
		speed = (targetZ >= MIN_DIST_TO_MESH) ? relSpeed : 0;
	}

	return speed; 
}

	 
