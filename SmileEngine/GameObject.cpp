#include "GameObject.h"
#include "Component.h"
#include "ComponentMesh.h"
#include "ComponentTransform.h"
#include "ComponentCamera.h"
#include "Glew/include/GL/glew.h" 
#include "SmileSpatialTree.h"
#include "SmileApp.h"
#include "imgui/imgui.h"
#include "imgui/ImGuizmo.h"


GameObject::GameObject(GameObject* parent)
{
	// Parent-child
	if (parent)
		SetParent(parent);

	// Components
	FillComponentBuffers();
	AddComponent((Component*)DBG_NEW ComponentTransform());
}

GameObject::GameObject(std::string name, GameObject* parent)
{
	// Parent-child
	if (parent)
		SetParent(parent);

	// Name after assigning parent
	SetName(name); 

	// Components
	FillComponentBuffers(); 
	AddComponent((Component*) DBG_NEW ComponentTransform());

}

GameObject::GameObject(Component* comp, std::string name, GameObject* parent)
{
	// Parent-child
	if (parent)
		SetParent(parent);

	// Name after assigning parent
	SetName(name);

	// Components
	FillComponentBuffers();
	if (comp->type != TRANSFORM)
		AddComponent((Component*)DBG_NEW ComponentTransform());

	AddComponent(comp); 
}


GameObject::GameObject(std::vector<Component*> components, std::string name, GameObject* parent)
{
	SetName(name); 

	// Components
	FillComponentBuffers();

	bool foundTransform = false; 
	
	for (auto& comp : components)
	{
		if (comp->type == TRANSFORM)
			foundTransform = true; 

		AddComponent(comp); 
	}

	if (foundTransform == false)
		AddComponent((Component*)DBG_NEW ComponentTransform());
}

void GameObject::FillComponentBuffers() // needed in order to have either a Component or a vector of Components in each slot
{
	for (int i = 0; i <= COMPONENT_TYPE::MAX_COMPONENT_TYPES - 1; ++i)
		components[i] = (Component*)NULL;
}

void GameObject::Start()
{
	active = true;

	// 0) Logic 
	if (GetMesh() == nullptr)
		SetupBounding();
	else
		SetupWithMesh();

	// 1) Components
	for (auto& comp : components)
		if (comp)
				comp->Start();
	

	// 2) Children (gameObjects)
	for (auto& obj : childObjects)
			obj->Start();
}

void GameObject::Enable()
{
	active = true;

	// 1) Components
	for (auto& comp : components)
		if (comp)
			if (comp->active == false)
				comp->Enable();

		
    // 2) Children (gameObjects)
	for (auto& obj : childObjects)
		if (obj->IsActive() == false)
			obj->Enable();

}

void GameObject::Disable()
{
	active = false;

	// 1) Components
	for (auto& comp : components)
		if (comp)
			if (comp->active == true)
				comp->Disable();

	// 2) Children (gameObjects)
	for (auto& obj : childObjects)
		if(obj->IsActive())
			obj->Disable();
}

void GameObject::Update()
{
	if (active == false)
		return; 

	// 1) Components
	for (auto& comp : components)
		if (comp)
			if (comp->active == true)
				comp->Update();

	// 2) Children (gameObjects)
	for (auto& obj : childObjects)
		if(obj->IsActive())
			obj->Update();
	
	// Lastly debug stuff :) 
	Debug(); 
}

void GameObject::DrawAxis()
{
	math::float3 transfPos = dynamic_cast<ComponentTransform*>(GetComponent(TRANSFORM))->GetGlobalPosition(); 
	glLineWidth(5); 
	glBegin(GL_LINES);

	// Line
	glColor3f(1, 0, 0);
	glVertex3f((GLfloat)transfPos.x, (GLfloat)transfPos.y, (GLfloat)transfPos.z);
	glVertex3f((GLfloat)transfPos.x + (GLfloat)debugLineSize, (GLfloat)transfPos.y, (GLfloat)transfPos.z);
	
	// Head
	glVertex3f((GLfloat)transfPos.x + (GLfloat)debugLineSize, (GLfloat)transfPos.y, (GLfloat)transfPos.z);
	glVertex3f((GLfloat)transfPos.x + (GLfloat)debugLineSize - (GLfloat)debugLineHead, (GLfloat)transfPos.y, (GLfloat)transfPos.z + (GLfloat)debugLineHead);
	glVertex3f((GLfloat)transfPos.x + (GLfloat)debugLineSize, (GLfloat)transfPos.y, (GLfloat)transfPos.z);
	glVertex3f((GLfloat)transfPos.x + (GLfloat)debugLineSize - (GLfloat)debugLineHead, (GLfloat)transfPos.y, (GLfloat)transfPos.z - (GLfloat)debugLineHead);

	// Line
	glColor3f(0, 1, 0);
	glVertex3f((GLfloat)transfPos.x, (GLfloat)transfPos.y, (GLfloat)transfPos.z);
	glVertex3f((GLfloat)transfPos.x, (GLfloat)transfPos.y + (GLfloat)debugLineSize, (GLfloat)transfPos.z);

	// Head
	glVertex3f((GLfloat)transfPos.x, (GLfloat)transfPos.y + (GLfloat)debugLineSize, (GLfloat)transfPos.z);
	glVertex3f((GLfloat)transfPos.x + (GLfloat)debugLineHead, (GLfloat)transfPos.y + (GLfloat)debugLineSize - (GLfloat)debugLineHead, (GLfloat)transfPos.z);
	glVertex3f((GLfloat)transfPos.x, (GLfloat)transfPos.y + (GLfloat)debugLineSize, (GLfloat)transfPos.z);
	glVertex3f((GLfloat)transfPos.x - (GLfloat)debugLineHead, (GLfloat)transfPos.y + (GLfloat)debugLineSize - (GLfloat)debugLineHead, (GLfloat)transfPos.z - (GLfloat)debugLineHead);


	//Line
	glColor3f(0, 0, 1);
	glVertex3f((GLfloat)transfPos.x, (GLfloat)transfPos.y, (GLfloat)transfPos.z);
	glVertex3f((GLfloat)transfPos.x, (GLfloat)transfPos.y, (GLfloat)transfPos.z + (GLfloat)debugLineSize);

	// Head
	glVertex3f((GLfloat)transfPos.x, (GLfloat)transfPos.y, (GLfloat)transfPos.z + (GLfloat)debugLineSize);
	glVertex3f((GLfloat)transfPos.x + (GLfloat)debugLineHead, (GLfloat)transfPos.y, (GLfloat)transfPos.z + (GLfloat)debugLineSize - (GLfloat)debugLineHead);
	glVertex3f((GLfloat)transfPos.x, (GLfloat)transfPos.y, (GLfloat)transfPos.z + (GLfloat)debugLineSize);
	glVertex3f((GLfloat)transfPos.x - (GLfloat)debugLineHead, (GLfloat)transfPos.y, (GLfloat)transfPos.z + (GLfloat)debugLineSize - (GLfloat)debugLineHead);


	glEnd(); 
	glColor3f(1.0f, 1.0f, 1.0f);
	glLineWidth(1);
}

void GameObject::CleanUp()
{
	// 1) Components 
	for (auto& comp : components)
		if (comp)
		{
			comp->CleanUp();
			RELEASE(comp); 
		}
 

	// 2) Children (gameObjects)
	for (auto& obj : childObjects)
	{
		obj->CleanUp();
		RELEASE(obj); 
	}
	childObjects.clear(); 
	
}


bool GameObject::AddComponent(Component* comp)
{
	if (comp != nullptr)
	{
		// Check if there already exists a component of that type
		if (components[comp->type] != nullptr)
		{
			components[comp->type]->CleanUp();
			RELEASE(components[comp->type]);
		}

		components[comp->type] = comp;

		comp->SetParent(this); 
		comp->Enable();

		return true;
	}

	return false;
} 

void GameObject::SetParent(GameObject* parent)
{
	if (parent) 
	{ 
		this->parent = parent; 
		this->parent->childObjects.push_back(this); 
	};

}
 
std::vector<GameObject*> GameObject::GetChildrenRecursive() const
{
	std::vector<GameObject*> listChildren;

	for (const auto& child : this->childObjects)
	{
		listChildren.push_back(child);

		if (child->childObjects.size() > 0)
		{
			std::vector<GameObject*> listGrandChildren;
			listGrandChildren = child->GetChildrenRecursive();
			listChildren.insert(listChildren.end(), listGrandChildren.begin(), listGrandChildren.end());
		}
		
	}

	return listChildren;
}

std::vector<GameObject*> GameObject::GetImmidiateChildren() const
{
	return childObjects; 
}

void GameObject::OnTransform(bool updateBounding) // called when the Transform sets a new global matrix
{
	// Update the bounding boxes -> right now false only when you setup the boxes, and then locate the transform at the 
	// OBB center. In that case, the bounding must not be overwritten. From thet point on, it is true
	if(updateBounding)
		UpdateBounding();

	// Update components if they need so (like camera)
	for (auto& comp : components)
		if (comp)
			comp->OnTransform();
}

float GameObject::GetBoundingSphereRadius() const // The object radius = the mesh's radius. In case of not having a mesh, returns 0
{
	auto mesh = dynamic_cast<ComponentMesh*>(components[MESH]); 

	if (mesh != nullptr)
		return boundingData.OBB.HalfDiagonal().Length();

	return 0; 
}

void GameObject::SetName(std::string name)
{
	if (name == "")
		if(parent && parent->childObjects.size() > 0)
			name = parent->GetName() + std::string(" (") + std::to_string(parent->childObjects.size()) + std::string(")");
	
	this->name = name;
}

ComponentTransform* GameObject::GetTransform() const
{
	return dynamic_cast<ComponentTransform*>(components[TRANSFORM]);
}

ComponentMesh* GameObject::GetMesh() const
{
	return dynamic_cast<ComponentMesh*>(components[MESH]);
}

ComponentCamera* GameObject::GetCamera() const
{
	return dynamic_cast<ComponentCamera*>(components[CAMERA]);
}

void GameObject::SetupWithMesh()
{
	//PositionTransformAtMeshCenter();
	SetupBounding(); 
}

static float3 GetMidPoint(float* vertex, uint num_vertex)
{
	float minmax[6]; 

	// 1) find the min-max coords
	for (uint i = 0; i < num_vertex; i += 3)
	{
		// first, initialize the min-max coords to the first vertex, 
		// in order to compare the following ones with it
		if (i == 0)
		{
			minmax[0] = vertex[i];
			minmax[1] = vertex[i];
			minmax[2] = vertex[i + 1];
			minmax[3] = vertex[i + 1];
			minmax[4] = vertex[i + 2];
			minmax[5] = vertex[i + 2];
			continue;
		}

		// find min-max X coord
		if (vertex[i] < minmax[0])
			minmax[0] = vertex[i];
		else if (vertex[i] > minmax[1])
			minmax[1] = vertex[i];

		// find min-max Y coord
		if (vertex[i + 1] < minmax[2])
			minmax[2] = vertex[i + 1];
		else if (vertex[i + 1] > minmax[3])
			minmax[3] = vertex[i + 1];

		// find min-max Z coord
		if (vertex[i + 2] < minmax[4])
			minmax[4] = vertex[i + 2];
		else if (vertex[i + 2] > minmax[5])
			minmax[5] = vertex[i + 2];

	}

	// 2) find the center 
	float c_X = (minmax[0] + minmax[1]) / 2;
	float c_Y = (minmax[1] + minmax[2]) / 2;
	float c_Z = (minmax[3] + minmax[4]) / 2;
	
	return float3(c_X, c_Y, c_Z);
}

void GameObject::PositionTransformAtMeshCenter()
{
	auto mesh = dynamic_cast<ComponentMesh*>(components[MESH]);
	auto transform = dynamic_cast<ComponentTransform*>(components[TRANSFORM]);
	// Setup transform local position to mesh center: do not update bounding box, previously calculated!!
	if (mesh != nullptr && transform != nullptr)
	{
		float3 meshGlobalPos = /*parent->GetTransform()->GetGlobalPosition() + */GetMidPoint(mesh->GetMeshData()->vertex, mesh->GetMeshData()->num_vertex); 
		transform->ChangePosition(meshGlobalPos, true, false);
		// the mesh vertices need to be updated now!! (to be relative to the new transform)
		//mesh->ReLocateMeshVertices();
	}

}

void GameObject::SetupBounding()  
{
	float4x4 transfGlobalMat = GetTransform()->GetGlobalMatrix();

	// No child objects = case A) 
	if (childObjects.size() == 0)
	{
		ModelMeshData* data = GetMesh()->GetMeshData(); 
		if (data)
		{
			// Setup a fake AABB, at first setup with no min-max coords, then build it upon the mesh vertex buffer
			math::AABB temp; 
			temp.SetNegativeInfinity();
			temp.Enclose((math::float3*)data->vertex, data->num_vertex);

			// Now the fake AABB has proper min-max coords, copy it to the OBB. Then, rotate it  
			boundingData.OBB = temp; 
			boundingData.OBB.Transform(transfGlobalMat);

			// Now calculate the real AABB: it must "contain" or "encompass" the OBB
			boundingData.AABB.SetNegativeInfinity();
			boundingData.AABB.Enclose(boundingData.OBB); 
	
		}

	}
	else
	{
		transfGlobalMat = GetTransform()->GetGlobalMatrix();

		// what here haha -> for the mom an arbitrarily-sized box at the right location
		boundingData.OBB.SetNegativeInfinity(); 
		boundingData.OBB.SetFrom((math::Sphere(transfGlobalMat.TranslatePart(), 0.3))); 
		boundingData.AABB.SetNegativeInfinity();
		boundingData.AABB.Enclose(boundingData.OBB);
	}
}

void GameObject::UpdateBounding()
{
	float4x4 transfGlobalMat = GetTransform()->GetGlobalMatrix();

	if (boundingData.AABB.IsDegenerate() == true || boundingData.OBB.IsDegenerate() == true
		|| transfGlobalMat.IsFinite() == false)
		return; 

	// This should work:
	SetupBounding(); 
}

void GameObject::Debug()
{
	// TODO: AABB and OBB debug, for the mom vertices 

	if (this->debugData.OBB)
	{
		glPointSize(10);
		glColor3f(0, 0, 1);
		glBegin(GL_POINTS);

		float3 vertices[8]; 
		boundingData.OBB.GetCornerPoints(vertices);
		for (auto& vertex : vertices)
			glVertex3f(vertex.x, vertex.y, vertex.z); 

		glEnd();
		glColor3f(1, 1, 1);
		glPointSize(1);

	}
	

}

void GameObject::SetStatic(bool isStatic)
{ 
	this->isStatic = isStatic; 
	App->spatial_tree->OnStaticChange(this, this->isStatic);
}

void GameObject::Draw()
{
    ComponentMesh* mesh = GetMesh(); if (mesh) mesh->Draw(); 
}

void GameObject::ShowTransformInspector()
{
	KEY_STATE keyState = App->input->GetKey(SDL_SCANCODE_KP_ENTER);

auto GetStringFrom3Values = [](float3 xyz, bool append) -> std::string
{
	return std::string(
		std::string((append) ? "(" : "") + std::to_string(xyz.x)
		+ std::string((append) ? ", " : "") + std::to_string(xyz.y)
		+ std::string((append) ? ", " : "") + std::to_string(xyz.z))
		+ std::string((append) ? ")" : "");
};

	ComponentTransform* transf = GetTransform(); 
	math::float3 pos = transf->GetPosition();
	math::float3 rot = transf->GetRotation().ToEulerXYZ();
	math::float3 degRot = RadToDeg(rot);
	math::float3 sc = transf->GetScale();
	float p[3] = { pos.x, pos.y, pos.z };
	float r[3] = { degRot.x, degRot.y, degRot.z };
	float s[3] = { sc.x, sc.y, sc.z };
	ImGui::InputFloat3("Position", p); // , -500, 500);
	ImGui::InputFloat3("Rotation", r); // , -359.999, 359.999);
	ImGui::InputFloat3("Scale", s); // , 0, 500);

	// (info)
	ImGui::Text(std::string("Global Center: " + GetStringFrom3Values(transf->GetGlobalPosition(), true)).c_str());

	if (keyState != KEY_DOWN)
		return;							

	math::float3 radRot = math::DegToRad(math::float3(r[0], r[1], r[2]));
	float radR[3] = { radRot.x, radRot.y, radRot.z };

	float values[3][3] = { {p[0], p[1], p[2]}, {radR[0], radR[1], radR[2]} , {s[0], s[1], s[2]} };
	transf->UpdateTransform(values);
}