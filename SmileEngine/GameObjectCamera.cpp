#include "SmileSetup.h"
#include "SmileApp.h" 
#include "GameObjectCamera.h"  
#include "SafetyHandler.h"
#include "RayTracer.h"

#include "GameObject.h"
#include "Component.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"

static void CommonSetup(GameObjectCamera* callback)
{
	callback->SetName("Camera"); 
	callback->X = vec3(1.0f, 0.0f, 0.0f);
	callback->Y = vec3(0.0f, 1.0f, 0.0f);
	callback->Z = vec3(0.0f, 0.0f, 1.0f);
}; 

GameObjectCamera::GameObjectCamera(GameObject* parent) : GameObject(parent)
{
	AddComponent(DBG_NEW ComponentTransform()); 
	CalculateViewMatrix();
	CommonSetup(this);
	Reference = vec3(0.0f, 0.0f, 0.0f);

	// Frustrum 
	frustrum = DBG_NEW Frustrum(this); 
}


GameObjectCamera::GameObjectCamera(GameObject* parent, vec3 Position, vec3 Reference) : GameObject(parent)
{
	ComponentTransform* transf = DBG_NEW ComponentTransform(math::float3(Position.x, Position.y, Position.z)); 
	AddComponent((Component*)transf);
	CalculateViewMatrix();
	CommonSetup(this);
	this->Reference = Reference; 

	// Frustrum 
	frustrum = DBG_NEW Frustrum(this);
}

GameObjectCamera::~GameObjectCamera()
{
	RELEASE(frustrum); 
}

// ----------------------------------------------------------------- 
// This must be done before clearing the Depth buffer in the Render PreUpdate
void GameObjectCamera::Update()
{
	float dt = App->GetDT(); 
	ComponentTransform* transf = GetTransform(); 

	// Check if the user clicks to select object 
	if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN)
		rayTracer::MouseOverMesh(App->input->GetMouseX(), App->input->GetMouseY(), true, true);

	if (App->gui->IsMouseOverTheGui() == false)
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
			newPos += X * speed * sMath::Sign(xWheel);

		if (yWheel != 0 && (mouseMiddle == KEY_DOWN || mouseMiddle == KEY_REPEAT))
			newPos += Y * speed * sMath::Sign(yWheel);

		if (zScroll != 0)
			newPos += Z * GetScrollSpeed(dt, zScroll);

		GetTransform()->AccumulatePosition(newPos); 
		Reference += newPos;

		// Rotation ----------------
		if (App->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT && App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT)
		{
			float Sensitivity = 0.25f;

			transf->AccumulatePosition(-Reference);

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

			vec3 transfPos = transf->GetPositionVec3(); 
			vec3 target = Reference + Z * length(transfPos); 
			transf->ChangePosition(math::float3(target.x, target.y, target.z));
		}
	}

	// Recalculate matrix -------------
	CalculateViewMatrix();


}

void GameObjectCamera::FocusObjectLogic()
{
	ComponentMesh* selectedMesh = App->scene_intro->selected_mesh;
	GameObject* selectedObj = App->scene_intro->selectedObj;

	bool rotating = (App->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT && App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT
		&& (App->gui->IsMouseOverTheGui() == false)) ? true : false;

	if (selectedMesh != nullptr)
	{
		if (App->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN)
			FitCameraToObject(selectedMesh->GetParent());

		if (rotating)
			LookAt(dynamic_cast<ComponentTransform*>(selectedMesh->GetParent()->GetComponent(TRANSFORM))->GetPosition());

	}
	else if (selectedObj != nullptr)
	{
		if (App->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN)
			FitCameraToObject(selectedObj);

		if (rotating)
			LookAt(dynamic_cast<ComponentTransform*>(selectedObj->GetComponent(TRANSFORM))->GetPosition());

	}
}

// -----------------------------------------------------------------
void GameObjectCamera::Look(const vec3& Position, const vec3& Reference, bool RotateAroundReference)
{
	ComponentTransform* transf = GetTransform(); 
	transf->ChangePosition(math::float3(Position.x, Position.y, Position.z));
	this->Reference = Reference;

	Z = normalize(Position - Reference);
	X = normalize(cross(vec3(0.0f, 1.0f, 0.0f), Z));
	Y = cross(Z, X);

	if (!RotateAroundReference)
	{
		this->Reference = transf->GetPositionVec3();
		transf->AccumulatePosition(Z * 0.05f);
	}

	CalculateViewMatrix();
}

// -----------------------------------------------------------------
void GameObjectCamera::LookAt(const float3& Spot)
{
	vec3 SpotV(Spot.x, Spot.y, Spot.z);

	Reference = SpotV;
  
	Z = normalize(GetTransform()->GetPositionVec3() - Reference);
	X = normalize(cross(vec3(0.0f, 1.0f, 0.0f), Z));
	Y = cross(Z, X);

	CalculateViewMatrix();
}


// -----------------------------------------------------------------
void GameObjectCamera::LookAt(const vec3& Spot)
{
	ComponentTransform* transf = GetTransform();
	Reference = Spot;

	Z = normalize(transf->GetPositionVec3() - Reference);
	X = normalize(cross(vec3(0.0f, 1.0f, 0.0f), Z));
	Y = cross(Z, X);

	CalculateViewMatrix();
}

// -----------------------------------------------------------------
void GameObjectCamera::Move(const vec3& Movement)
{
	ComponentTransform* transf = GetTransform();
	transf->AccumulatePosition(Movement);
	Reference += Movement;

	CalculateViewMatrix();
}

// -----------------------------------------------------------------
float* GameObjectCamera::GetViewMatrix()
{
	return &ViewMatrix;
}


// -----------------------------------------------------------------
float* GameObjectCamera::GetViewMatrixInverse()
{
	return &ViewMatrixInverse;
}

// -----------------------------------------------------------------
void GameObjectCamera::CalculateViewMatrix()
{
	vec3 transfPos = GetTransform()->GetPositionVec3();
	ViewMatrix = mat4x4(X.x, Y.x, Z.x, 0.0f, X.y, Y.y, Z.y, 0.0f, X.z, Y.z, Z.z, 0.0f, -dot(X, transfPos), -dot(Y, transfPos), -dot(Z, transfPos), 1.0f);
	ViewMatrixInverse = inverse(ViewMatrix);
}

// -----------------------------------------------------------------
void GameObjectCamera::FitCameraToObject(GameObject* obj)
{

	vec3 transfPos = GetTransform()->GetPositionVec3();

	// look at the mesh center
	float3 centerMath = dynamic_cast<ComponentTransform*>(obj->GetComponent(TRANSFORM))->GetPosition();
	vec3 center(centerMath.x, centerMath.y, centerMath.z);

	// we will need the bounding sphere radius
	LookAt(center);

	// calculate the distance with the center
	double camDistance = (obj->GetBoundingSphereRadius()) / math::Tan(math::DegToRad(App->renderer3D->GetData().fovYangle / 2.F));
	math::float3 dir = (math::float3(transfPos.x, transfPos.y, transfPos.z) - math::float3(center.x, center.y, center.z));
	dir.Normalize();
	vec3 dirVec3(dir.x, dir.y, dir.z);

	// the targed pos (of the camera from the mesh) is the center plus the direction by the distance.
	vec3 wantedPos = center + dirVec3 * camDistance;

	// finally cap it 
	if (abs((wantedPos - center).z) < MIN_DIST_TO_MESH)
		wantedPos.z = MIN_DIST_TO_MESH;

	Move(vec3(wantedPos - transfPos));

}

// -----------------------------------------------------------------
float GameObjectCamera::GetScrollSpeed(float dt, float zScroll)
{
	vec3 transfPos = GetTransform()->GetPositionVec3();

	float speed = DEFAULT_SPEED * dt * sMath::Sign(zScroll);

	ComponentMesh* selectedMesh = App->scene_intro->selected_mesh;
	GameObject* selectedObj = App->scene_intro->selectedObj;

	GameObject* target = (selectedMesh == nullptr) ? selectedObj : selectedMesh->GetParent();
	if (target != nullptr)
	{
		// some stupid conversions from vec3 to float3
		float3 captureCamPos(transfPos.x, transfPos.y, transfPos.z);
		float3 captureZ(Z.x, Z.y, Z.z);

		float relSpeed = pow((abs((captureCamPos - dynamic_cast<ComponentTransform*>(target->GetComponent(TRANSFORM))->GetPosition()).Length())), EXPONENTIAL_ZOOM_FACTOR)* dt* sMath::Sign(zScroll);
		relSpeed = (relSpeed > MAX_FRAME_SPEED) ? MAX_FRAME_SPEED : relSpeed;

		float targetZ = abs((captureCamPos + captureZ * relSpeed).z);
		speed = (targetZ >= MIN_DIST_TO_MESH) ? relSpeed : 0;
	}

	return speed;
}


// -----------------------------------------------------------------
// -----------------------------------------------------------------
// ----------------------------------------------------------------- [Frustrum]
Frustrum::Frustrum(GameObjectCamera* camera)
{
	myCamera = camera; 
	Frustrum::CalculatePlanes(); 
}

void Frustrum::CalculatePlanes()
{
	// Step 1: retrieve render data and the camera looking direction (-Z for the mom) 
	renderingData renderData = App->renderer3D->GetData(); 
	float3 camLookVec = float3(0, 0, -1); float2 Up = float2(0, 1), Right = float2(1, 0);  // TODO: camera rotations!!                                           
	float3 camPos = myCamera->GetTransform()->GetPosition();  
	float2 camPos2d = float2(camPos.x, camPos.y); 

	// Step 2: Get the middle point (center) of the near plane and the far plane
	float3 middleNearPlanePoint = camPos + camLookVec * renderData.pNearDist;
	float3 middleFarPlanePoint = camPos + camLookVec * renderData.pFarDist;
	float2 middleNearPlanePoint2D = float2(middleNearPlanePoint.x, middleNearPlanePoint.y); 
	float2 middleFarPlanePoint2D = float2(middleFarPlanePoint.x, middleFarPlanePoint.y);

	// Step 3: construct the 4 vertices around the center with quick sassy math
	
	// Near Plane
	plane nPlane; 

	float2 nPlanepPoint1_2D = float2(middleNearPlanePoint2D + (Right * renderData.pNearSize.x / 2) - (Up * renderData.pNearSize.y / 2));
	nPlane.vertices[0] = float3(nPlanepPoint1_2D, renderData.pNearDist);
	
	float2 nPlanepPoint2_2D = float2(middleNearPlanePoint2D + (Right * renderData.pNearSize.x / 2) + (Up * renderData.pNearSize.y / 2));
	nPlane.vertices[1] = float3(nPlanepPoint2_2D, renderData.pNearDist);
	
	float2 nPlanepPoint3_2D = float2(middleNearPlanePoint2D - (Right * renderData.pNearSize.x / 2) + (Up * renderData.pNearSize.y / 2));
	nPlane.vertices[2] = float3(nPlanepPoint3_2D, renderData.pNearDist);
	
	float2 nPlanepPoint4_2D = float2(middleNearPlanePoint2D - (Right * renderData.pNearSize.x / 2) - (Up * renderData.pNearSize.y / 2));
	nPlane.vertices[3] = float3(nPlanepPoint4_2D, renderData.pNearDist);

	planes[0] = nPlane; 

	// Far Plane
	plane fPlane;

	float2 fPlanepPoint1_2D = float2(middleFarPlanePoint2D + (Right * renderData.pFarSize.x / 2) - (Up * renderData.pFarSize.y / 2));
	fPlane.vertices[0] = float3(fPlanepPoint1_2D, renderData.pFarDist);

	float2 fPlanepPoint2_2D = float2(middleFarPlanePoint2D + (Right * renderData.pFarSize.x / 2) + (Up * renderData.pFarSize.y / 2));
	fPlane.vertices[1] = float3(fPlanepPoint2_2D, renderData.pFarDist);

	float2 fPlanepPoint3_2D = float2(middleFarPlanePoint2D - (Right * renderData.pFarSize.x / 2) + (Up * renderData.pFarSize.y / 2));
	fPlane.vertices[2] = float3(fPlanepPoint3_2D, renderData.pFarDist);

	float2 fPlanepPoint4_2D = float2(middleFarPlanePoint2D - (Right * renderData.pFarSize.x / 2) - (Up * renderData.pFarSize.y / 2));
	fPlane.vertices[3] = float3(fPlanepPoint4_2D, renderData.pFarDist);

	planes[1] = fPlane;

	// Step 4: figure out the rest of the planes 
	// simplest approach is look at the 1,2,3,4 order of the near and far planes and pick vertices

	// Right Plane 
	plane rPlane; 
	rPlane.vertices[0] = nPlane.vertices[0]; 
	rPlane.vertices[1] = fPlane.vertices[0];
	rPlane.vertices[2] = fPlane.vertices[1];
	rPlane.vertices[3] = nPlane.vertices[1];
	planes[2] = rPlane;

	// Left Plane 
	plane lPlane;
	lPlane.vertices[0] = fPlane.vertices[3];
	lPlane.vertices[1] = fPlane.vertices[2];
	lPlane.vertices[2] = nPlane.vertices[2];
	lPlane.vertices[3] = nPlane.vertices[3];
	planes[3] = lPlane;

	// Top Plane 
	plane tPlane;
	tPlane.vertices[0] = nPlane.vertices[1];
	tPlane.vertices[1] = fPlane.vertices[1];
	tPlane.vertices[2] = fPlane.vertices[2];
	tPlane.vertices[3] = nPlane.vertices[2];
	planes[4] = tPlane;

	// Bottom Plane 
	plane bPlane;
	bPlane.vertices[0] = nPlane.vertices[0];
	bPlane.vertices[1] = fPlane.vertices[0];
	bPlane.vertices[2] = fPlane.vertices[3];
	bPlane.vertices[3] = nPlane.vertices[3];
	planes[5] = bPlane;
}