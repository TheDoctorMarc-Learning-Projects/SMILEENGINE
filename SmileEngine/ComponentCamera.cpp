#include "SmileSetup.h"
#include "SmileApp.h" 
#include "ComponentCamera.h"  
#include "SafetyHandler.h"
#include "RayTracer.h"

#include "GameObject.h"
#include "Component.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"

static void CommonSetup(ComponentCamera* callback)
{
	callback->SetName("Camera"); 
	callback->X = vec3(1.0f, 0.0f, 0.0f);
	callback->Y = vec3(0.0f, 1.0f, 0.0f);
	callback->Z = vec3(0.0f, 0.0f, 1.0f);
}; 

ComponentCamera::ComponentCamera(GameObject* parent, vec3 Reference, renderingData data) : Component(parent)
{
	type = COMPONENT_TYPE::CAMERA;
	SetName("Camera comp"); // TODO: change this once the hierachy is tree-like

	// Setup
	CommonSetup(this);
	this->Reference = Reference;

	// Calculate view
	CalculateViewMatrix();
	LookAt(this->Reference);

	// Compute Data before frustrum
	this->_renderingData = data;
	ComputeSpatialData();

	// Frustrum 
	frustrum = DBG_NEW Frustrum(this);
}


ComponentCamera::~ComponentCamera()
{
	RELEASE(frustrum); 
}

float renderingData::InitRatio()
{
	return (float)(std::get<int>(App->window->GetWindowParameter("Width"))) / (float)(std::get<int>(App->window->GetWindowParameter("Height"))); 
}

void ComponentCamera::ComputeSpatialData()
{
	// Near plane size
	_renderingData.pNearSize.y = abs(2 * tan((_renderingData.fovYangle * DEGTORAD) / 2) * _renderingData.pNearDist);
	_renderingData.pNearSize.x = _renderingData.pNearSize.y * _renderingData.ratio;

	// Far plane size
	_renderingData.pFarSize.y = abs(2 * tan((_renderingData.fovYangle * DEGTORAD) / 2) * _renderingData.pFarDist);
	_renderingData.pFarSize.x = _renderingData.pFarSize.y * _renderingData.ratio;
}

// ----------------------------------------------------------------- 
void ComponentCamera::Update()
{
	// If the current looking camera is not myself, fuck the logic!
	if (App->renderer3D->targetCamera != this)
	{
		frustrum->DebugPlanes();
		return;
	}
	
	float dt = App->GetDT(); 
	ComponentTransform* transf = parent->GetTransform(); 

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

		parent->GetTransform()->AccumulatePosition(newPos);
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

void ComponentCamera::FocusObjectLogic()
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
void ComponentCamera::Look(const vec3& Position, const vec3& Reference, bool RotateAroundReference)
{
	ComponentTransform* transf = parent->GetTransform();
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
void ComponentCamera::LookAt(const float3& Spot)
{
	vec3 SpotV(Spot.x, Spot.y, Spot.z);

	Reference = SpotV;
  
	Z = normalize(parent->GetTransform()->GetPositionVec3() - Reference);
	X = normalize(cross(vec3(0.0f, 1.0f, 0.0f), Z));
	Y = cross(Z, X);

	CalculateViewMatrix();
}


// -----------------------------------------------------------------
void ComponentCamera::LookAt(const vec3& Spot)
{
	ComponentTransform* transf = parent->GetTransform();
	Reference = Spot;

	Z = normalize(transf->GetPositionVec3() - Reference);
	X = normalize(cross(vec3(0.0f, 1.0f, 0.0f), Z));
	Y = cross(Z, X);

	CalculateViewMatrix();
}

// -----------------------------------------------------------------
void ComponentCamera::Move(const vec3& Movement)
{
	ComponentTransform* transf = parent->GetTransform();
	transf->AccumulatePosition(Movement);
	Reference += Movement;

	CalculateViewMatrix();
}

// -----------------------------------------------------------------
float* ComponentCamera::GetViewMatrix()
{
	return &ViewMatrix;
}


// -----------------------------------------------------------------
float* ComponentCamera::GetViewMatrixInverse()
{
	return &ViewMatrixInverse;
}

// -----------------------------------------------------------------
void ComponentCamera::CalculateViewMatrix(bool updateTransform)
{
	vec3 transfPos = parent->GetTransform()->GetPositionVec3();
	ViewMatrix = mat4x4(X.x, Y.x, Z.x, 0.0f, X.y, Y.y, Z.y, 0.0f, X.z, Y.z, Z.z, 0.0f, -dot(X, transfPos), -dot(Y, transfPos), -dot(Z, transfPos), 1.0f);
	ViewMatrixInverse = inverse(ViewMatrix);

	if (updateTransform)
	{
		float data[16];
		for (int i = 0; i <= 15; ++i)
			data[i] = ViewMatrix.M[i];
		float4x4 mat = float4x4(data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7], data[8], data[9], data[10], data[11], data[12], data[13], data[14], data[15]);
		parent->GetTransform()->ChangeRotation(Quat(mat));
	}

}

// -----------------------------------------------------------------
void ComponentCamera::FitCameraToObject(GameObject* obj)
{

	vec3 transfPos = parent->GetTransform()->GetPositionVec3();

	// look at the mesh center
	float3 centerMath = dynamic_cast<ComponentTransform*>(obj->GetComponent(TRANSFORM))->GetPosition();
	vec3 center(centerMath.x, centerMath.y, centerMath.z);

	// we will need the bounding sphere radius
	LookAt(center);

	// calculate the distance with the center
	double camDistance = (obj->GetBoundingSphereRadius()) / math::Tan(math::DegToRad(_renderingData.fovYangle / 2.F));
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
float ComponentCamera::GetScrollSpeed(float dt, float zScroll)
{
	vec3 transfPos = parent->GetTransform()->GetPositionVec3();

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
void ComponentCamera::OnTransform(bool transfData[3])
{
	// rotation
	if (transfData[2])
	{
		Quat q = parent->GetTransform()->GetRotation();
		float4x4 mat = float4x4(q).Transposed();
		
		X = normalize(vec3(mat.Row3(0).x, mat.Row3(0).y, mat.Row3(0).z)); 
		Y = normalize(vec3(mat.Row3(1).x, mat.Row3(1).y, mat.Row3(1).z));
		Z = normalize(vec3(mat.Row3(2).x, mat.Row3(2).y, mat.Row3(2).z));
	}

	CalculateViewMatrix(false); 
	frustrum->CalculatePlanes();
}

// -----------------------------------------------------------------
// -----------------------------------------------------------------
// ----------------------------------------------------------------- [Frustrum]
Frustrum::Frustrum(ComponentCamera* camera)
{
	myCamera = camera; 
	Frustrum::CalculatePlanes(); 
}

void Frustrum::CalculatePlanes()
{
	// Step 1: retrieve render data and the camera looking direction
	renderingData renderData = myCamera->GetRenderingData(); 
	float3 camLookVec = float3(myCamera->Z.x, myCamera->Z.y, -myCamera->Z.z).Normalized(),
		Right = (Cross(float3(0.0f, 1.0f, 0.0f), camLookVec)).Normalized(),
		Up = Cross(camLookVec, -Right); 
	float3 camPos = myCamera->GetParent()->GetTransform()->GetPosition();

	// Step 2: Get the middle point (center) of the near plane and the far plane
	float3 middleNearPlanePoint = camPos + camLookVec * renderData.pNearDist;
	float3 middleFarPlanePoint = camPos + camLookVec * renderData.pFarDist;

	// Step 3: construct the 4 vertices around the center with quick sassy math
	
	// Near Plane
	plane nPlane; 
	nPlane.vertices[0] = float3(middleNearPlanePoint + (Right * renderData.pNearSize.x / 2) - (Up * renderData.pNearSize.y / 2));
	nPlane.vertices[1] = float3(middleNearPlanePoint + (Right * renderData.pNearSize.x / 2) + (Up * renderData.pNearSize.y / 2));
	nPlane.vertices[2] = float3(middleNearPlanePoint - (Right * renderData.pNearSize.x / 2) + (Up * renderData.pNearSize.y / 2));
	nPlane.vertices[3] = float3(middleNearPlanePoint - (Right * renderData.pNearSize.x / 2) - (Up * renderData.pNearSize.y / 2));
	planes[0] = nPlane; 

	// Far Plane
	plane fPlane;
	fPlane.vertices[0] = float3(middleFarPlanePoint + (Right * renderData.pFarSize.x / 2) - (Up * renderData.pFarSize.y / 2));
	fPlane.vertices[1] = float3(middleFarPlanePoint + (Right * renderData.pFarSize.x / 2) + (Up * renderData.pFarSize.y / 2));
	fPlane.vertices[2] = float3(middleFarPlanePoint - (Right * renderData.pFarSize.x / 2) + (Up * renderData.pFarSize.y / 2));
	fPlane.vertices[3] = float3(middleFarPlanePoint - (Right * renderData.pFarSize.x / 2) - (Up * renderData.pFarSize.y / 2));
	planes[1] = fPlane;

	// Step 4: figure out the rest of the planes 
	// simplest approach is to look at the 0,1,2,3 order of the near and far planes' vertices and pick them

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

	// Step 5: Calculate the plane's normals and center 
	int i = 0; 
	for (auto& p : planes)
	{
		// normal
		float3 v1 = p.vertices[1] - p.vertices[0];
		float3 v2 = p.vertices[2] - p.vertices[1];  
		
		if (i % 2 == 0)
			p.normal = Cross(v2, v1);
		else
			p.normal = Cross(v1, v2);

		p.normal /= p.normal.Length(); 

		// center
		float3 flankDist = p.vertices[3] - p.vertices[0]; 
		float3 flankMid = p.vertices[0] + flankDist / 2; 
		float3 flank2Dist = p.vertices[2] - p.vertices[1];
		float3 flank2Mid = p.vertices[1] + flank2Dist / 2;
		float3 betweenFlanksDist = flank2Mid - flankMid; 
		p.center = flankMid + betweenFlanksDist / 2; 

		++i; 
	}; 

	
}

void Frustrum::DebugPlanes()
{
	glLineWidth(3);
	glColor3f(0, 1.f, 1.f); 
	glBegin(GL_LINES);

	// 4 lines from the camera to the far plane 
	float3 camPos = myCamera->GetParent()->GetTransform()->GetPosition();
	
	for (int i = 0; i <= 3; ++i)
	{
		glVertex3f((GLfloat)camPos.x, (GLfloat)camPos.y, (GLfloat)camPos.z);
		glVertex3f((GLfloat)planes[1].vertices[i].x, (GLfloat)planes[1].vertices[i].y, (GLfloat)planes[1].vertices[i].z);
	}

	// the near and far planes
	for (int i = 0; i <= 1; ++i)
	{
		for (int j = 0; j <= 3; ++j)
		{
			float3 vertex = planes.at(i).vertices[j];
			float3 vertex2 = float3(0, 0, 0);

			if (j <= 2)
				vertex2 = planes.at(i).vertices[j + 1];
			else
				vertex2 = planes.at(i).vertices[0];

			glVertex3f((GLfloat)vertex.x, (GLfloat)vertex.y, (GLfloat)vertex.z);
			glVertex3f((GLfloat)vertex2.x, (GLfloat)vertex2.y, (GLfloat)vertex2.z);
		}
	}

	// The normal vectors
	glColor3f(1.f, 1.f, 0.f);
	for (auto& p : planes)
	{
		float factor = 3; 
		glVertex3f((GLfloat)p.center.x, (GLfloat)p.center.y, (GLfloat)p.center.z);
		glVertex3f((GLfloat)p.center.x + p.normal.x * factor, (GLfloat)p.center.y + p.normal.y * factor, (GLfloat)p.center.z + p.normal.z * factor);
   
		
	}

	glEnd();
	glColor3f(1.0f, 1.0f, 1.0f);
	glLineWidth(1);

	// the plane centers
	glColor3f(0.8f, 0.8f, 0.2f);
	glPointSize(10);
	glBegin(GL_POINTS);
	for (auto& p : planes)
		glVertex3f((GLfloat)p.center.x, (GLfloat)p.center.y, (GLfloat)p.center.z);
	glEnd();
	glPointSize(1);
	glColor3f(1.f, 1.f, 1.f);
}

Frustrum::INTERSECTION_TYPE Frustrum::IsCubeInsideFrustrumView(bounding::BoundingBox& box)
{
	Frustrum::INTERSECTION_TYPE type = Frustrum::INTERSECTION_TYPE::INSIDE; 

	// to debug a vertex with green color if inside all 6 planes, or in red otherwise: 
	std::array<bool, 8> insideOutside; 
	std::fill(std::begin(insideOutside), std::end(insideOutside), true);

	for (int i = 0; i < 6; ++i) // planes 
	{
		int insideCount = 0;
		int outsideCount = 0; 

		for (int j = 0; j < 8; ++j) // vertices in box
		{ 
			float3 vertex = box.vertices.at(j); 
			if (planes[i].GetIntersection(vertex) == INTERSECTION_TYPE::OUTSIDE) // outside here means behind 
			{
				outsideCount++;
				insideOutside[j] = false; 
			}
			
			else if (planes[i].GetIntersection(vertex) == INTERSECTION_TYPE::INSIDE)
				insideCount++; 
		}

		if (outsideCount == 8)
		{
			insideOutside.fill(false); 
			box.insideoutside = insideOutside; 
			return INTERSECTION_TYPE::OUTSIDE;
		}
			

		if (insideCount > 0 && insideCount < 8)
			type = INTERSECTION_TYPE::INTERSECT; 
    }

	box.insideoutside = insideOutside;
	return type; 
}

Frustrum::INTERSECTION_TYPE Frustrum::plane::GetIntersection(float3 vertex)
{
	float totalDist = normal.Dot(vertex - center); 
	if(totalDist >= 0)
		return INTERSECTION_TYPE::INSIDE;

	return INTERSECTION_TYPE::OUTSIDE; 
}