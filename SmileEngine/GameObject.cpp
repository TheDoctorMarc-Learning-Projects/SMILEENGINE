#include "GameObject.h"
#include "Component.h"
#include "ComponentMesh.h"
#include "ComponentTransform.h"
#include "Glew/include/GL/glew.h" 

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
	Enable(); 
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

void GameObject::OnTransform(bool data[3])
{
	for (auto& comp : components)
		if (comp)
			comp->OnTransform(data);

	for (auto& obj : childObjects)
		obj->OnTransform(data);

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

void GameObject::SetupWithMesh()
{
	SetupBounding(); 
	PositionTransformAtMeshCenter();
}


void GameObject::PositionTransformAtMeshCenter()
{
	auto mesh = dynamic_cast<ComponentMesh*>(components[MESH]);
	auto transform = dynamic_cast<ComponentTransform*>(components[TRANSFORM]);

	// Setup transform local position to mesh center 
	if (mesh != nullptr && transform != nullptr)
		transform->ChangePosition(boundingData.OBB.CenterPoint());
	else
		LOG("GameObject could not setup the transform: missing mesh")
}

// the object either: 
// A) The object has no child objects, so the obb is built upon the mesh vertex buffer.
// The enclosing AABB is directly computed
// B) The object has child objects. 
// Both the AABB and the OBB must be computed as the sum of all the objects' boxes  

void GameObject::SetupBounding()  
{
	// No child objects = case A) 
	if (childObjects.size() == 0)
	{
		ModelMeshData* data = GetMesh()->GetMeshData(); 
		if (data)
		{
			// Setup a fake AABB, at first setup with no min-max coords, then build it upon the mesh vertex buffer
			boundingData.AABB.SetNegativeInfinity();
			boundingData.AABB.Enclose((math::float3*)data->vertex, data->num_vertex);

			// Now the fake AABB has proper min-max coords, copy it to the OBB. Then, rotate it  
			boundingData.OBB.SetFrom(boundingData.AABB); 
			boundingData.OBB.Transform(GetTransform()->GetGlobalMatrix()); 

			// Now calculate the real AABB: it must "contain" or "encompass" the OBB
			boundingData.AABB.Enclose(boundingData.OBB); 
		}

	}
	else
	{
		// what here haha 
	}
}