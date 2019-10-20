#include "GameObject.h"
#include "Component.h"
#include "ComponentMesh.h"
#include "ComponentTransform.h"

GameObject::GameObject(std::string name, GameObject* parent)
{
	SetName(name); 

	// Components
	FillComponentBuffers(); 
	AddComponent((Component*) DBG_NEW ComponentTransform());

	// Parent-child
	if (parent)
		SetParent(parent);
}

GameObject::GameObject(Component* comp, std::string name, GameObject* parent)
{
	SetName(name); 

	// Components
	FillComponentBuffers();
	if (comp->type != TRANSFORM)
		AddComponent((Component*)DBG_NEW ComponentTransform());

	AddComponent(comp); 

	// Parent-child
	if (parent)
		SetParent(parent); 
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


	// Parent-child
	if (parent)
		SetParent(parent);
}

void GameObject::FillComponentBuffers() // needed in order to have either a Component or a vector of Components in each slot
{
	Component* emptyGuy = DBG_NEW Component(); 
	Component* emptyGuy2 = DBG_NEW Component();

	components[TRANSFORM] = emptyGuy;  // one
	components[MESH] = std::vector<Component*>(); // multiple
	components[MATERIAL] = emptyGuy2;  // placeholder
	components[LIGHT] = std::vector<Component*>(); // multiple
}


void GameObject::Enable()
{
	
	// 1) Components
	// The variant "components" holds either a Component* if index = 0 or a std list of Components* if index = 1
	for (auto& comp : components)
	{
		if (comp.index() == 0)
		{
			if (std::get<Component*>(comp)->active == false)
				std::get<Component*>(comp)->Enable();
		}
		
		else if (comp.index() == 1)
		{
			auto& vComp = std::get<std::vector<Component*>>(comp);
			for (auto& comp2 : vComp)
				if(comp2->active == false)
					comp2->Enable();	
		}
			
	}

    // 2) Children (gameObjects)
	for (auto& obj : childObjects)
		if (obj->IsActive() == false)
			obj->Enable();




	active = true; 
}

void GameObject::Disable()
{
	// 1) Components
	// The variant "components" holds either a Component* if index = 0 or a std list of Components* if index = 1
	for (auto& comp : components)
	{
		if (comp.index() == 0)
		{
			if (std::get<Component*>(comp)->active)
				std::get<Component*>(comp)->Disable();
		}
			
		else if (comp.index() == 1)
		{
			auto& vComp = std::get<std::vector<Component*>>(comp);
			for (auto& comp2 : vComp)
				if (comp2->active)
					comp2->Disable();
		}

	}

	// 2) Children (gameObjects)
	for (auto& obj : childObjects)
		if(obj->IsActive())
			obj->Disable();


	active = false;
}

void GameObject::Update()
{
	// 1) Components
	for (auto& comp : components)
	{
		if (comp.index() == 0)
		{
			if (std::get<Component*>(comp)->active)
				std::get<Component*>(comp)->Update();
		}

		else if (comp.index() == 1)
		{
			auto& vComp = std::get<std::vector<Component*>>(comp);
			for (auto& comp2 : vComp)
				if (comp2->active)
					comp2->Update();
		}

	}

	// 2) Children (gameObjects)
	for (auto& obj : childObjects)
		if(obj->IsActive())
			obj->Update();
	
}

void GameObject::CleanUp()
{
	// 1) Components
	for (auto& comp : components)
	{
		if (comp.index() == 0)
		{
			std::get<Component*>(comp)->CleanUp();
			RELEASE(std::get<Component*>(comp));
		}
			
		else if (comp.index() == 1)
		{
			auto& vComp = std::get<std::vector<Component*>>(comp);
			for (auto& comp2 : vComp)
			{
				comp2->CleanUp();                             
				RELEASE(comp2);
			}
			vComp.clear(); 
		}
	}

	//delete[] components; 


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
		// Check if the component can be added to a GameObject 
		if (std::get<0>(uniquenessMap.at(comp->type)) == false)
			return false; 

		// Check if there cannot be more than one instance of that component
		if (std::get<2>(uniquenessMap.at(comp->type)) == false)
		{
			// Check if there already exists a component of that type
			if (std::get<Component*>(components[comp->type]) != nullptr)
			{
				std::get<Component*>(components[comp->type])->CleanUp();
				RELEASE(std::get<Component*>(components[comp->type]));
			}
				
			std::get<Component*>(components[comp->type]) = comp;

			goto Enable; 
		}
		else // if there can me more than one, push it to that component type list 
			std::get<std::vector<Component*>>(components[comp->type]).push_back(comp); 
	
		Enable: 
		comp->parent = this;
		comp->Enable();

		return true;
	}

	return false;
}

bool GameObject::AddComponentToMesh(Component* comp, ComponentMesh* mesh)
{
	if (comp != nullptr && mesh != nullptr)
	{
		// Check if the component can be added to a Mesh 
		if (std::get<1>(uniquenessMap.at(comp->type)) == false)
			return false;

		// Check if there cannot be more than one instance of that component
		if (std::get<2>(uniquenessMap.at(comp->type)) == false)
		{
			// Check if there already exists a component of that type
			if (std::get<Component*>(mesh->components[comp->type]) != nullptr)
			{
				std::get<Component*>(mesh->components[comp->type])->CleanUp();
				RELEASE(std::get<Component*>(mesh->components[comp->type])); 
			}
			
			std::get<Component*>(mesh->components[comp->type]) = comp;

			goto Enable;
		}
		else // if there can me more than one, push it to that component type list 
			std::get<std::vector<Component*>>(mesh->components[comp->type]).push_back(comp);

		Enable:
		comp->parent = mesh; 
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

	// recalculate the transform 
	if (std::get<Component*>(components[TRANSFORM]) != nullptr)
		dynamic_cast<ComponentTransform*>(std::get<Component*>(components[TRANSFORM]))->CalculateAllMatrixes(); 

}
 
std::vector<GameObject*> GameObject::GetChildrenRecursive()
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
void GameObject::OnTransform()
{
	// 1) First transform myself
	dynamic_cast<ComponentTransform*>(std::get<Component*>(components[TRANSFORM]))->CalculateAllMatrixes(); 

	// 2) Then, transform components
	for (auto& comp : components)
	{
		if (comp.index() == 0)
			std::get<Component*>(comp)->OnTransform();

		else if (comp.index() == 1)
		{
			auto& vComp = std::get<std::vector<Component*>>(comp);
			for (auto& comp2 : vComp)
				comp2->OnTransform();
	
		}
	}

	// Lastly, transform child gameobjects
	for (auto& obj : childObjects)
			obj->OnTransform();

}