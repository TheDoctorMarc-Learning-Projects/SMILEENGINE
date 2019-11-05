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
	math::float3 transfPos = dynamic_cast<ComponentTransform*>(GetComponent(TRANSFORM))->GetPosition(); 
	glLineWidth(10); 
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

void GameObject::OnTransform()
{
	// 1) First transform myself
	dynamic_cast<ComponentTransform*>(components[TRANSFORM])->CalculateAllMatrixes(); 

	// 2) Then update the children (gameobjects) 's transform
	for (auto& obj : childObjects)
			obj->OnTransform();

}

double GameObject::GetBoundingSphereRadius() const // The object radius = the mesh's radius. In case of not having a mesh, returns 0
{
	auto mesh = dynamic_cast<ComponentMesh*>(components[MESH]); 

	if (mesh != nullptr)
		return mesh->GetMeshData()->GetMeshSphereRadius();
	else
		return (double)0; 
}

void GameObject::SetupTransformAtMeshCenter()
{
	// Asign the object a transform as the mesh center (computed before)
	auto mesh = dynamic_cast<ComponentMesh*>(components[MESH]); 

	if (mesh != nullptr)
	{
		ModelMeshData* mesh_info = mesh->GetMeshData(); 
		ComponentTransform* transObj = DBG_NEW ComponentTransform();
		math::float4x4 transfMat = math::float4x4::identity;
		transObj->ChangePosition(math::float3(mesh->GetMeshData()->GetMeshCenter().x, mesh_info->GetMeshCenter().y, mesh_info->GetMeshCenter().z));
		AddComponent(transObj);
	}
	else
		LOG("GameObject could not setup the transform: missing mesh")


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
