#include "GameObject.h"
#include "Component.h"
#include "ComponentMesh.h"
#include "ComponentTransform.h"

GameObject::GameObject()
{
	AddComponent((Component*) DBG_NEW ComponentTransform());
}

GameObject::GameObject(Component* comp)
{
	if (comp->type != TRANSFORM)
		GameObject::GameObject(); 

	AddComponent(comp); 
}


GameObject::GameObject(std::vector<Component*> components)
{
	bool foundTransform = false; 
	
	for (auto& comp : components)
	{
		if (comp->type == TRANSFORM)
			foundTransform = true; 

		AddComponent(comp); 
	}

	if (foundTransform == false)
		GameObject::GameObject(); 
}

void GameObject::Enable()
{
	// The variant "components" holds either a Component* if index = 0 or a std list of Components* if index = 1
	for (auto& comp : components)
	{
		if (comp.index() == 0) 
			std::get<Component*>(comp)->Enable(); 
		else if (comp.index() == 1)
		{
			auto& vComp = std::get<std::vector<Component*>>(comp);
			for (auto& comp2 : vComp)
				comp2->Enable(); 
		}
			
	}
	active = true; 
}

void GameObject::Disable()
{
	// The variant "components" holds either a Component* if index = 0 or a std list of Components* if index = 1
	for (auto& comp : components)
	{
		if (comp.index() == 0)
			std::get<Component*>(comp)->Disable();
		else if (comp.index() == 1)
		{
			auto& vComp = std::get<std::vector<Component*>>(comp);
			for (auto& comp2 : vComp)
				comp2->Disable();
		}

	}
	active = false;
}

void GameObject::Update()
{
	for (auto& comp : components)
	{
		if (comp.index() == 0)
			std::get<Component*>(comp)->Update();
		else if (comp.index() == 1)
		{
			auto& vComp = std::get<std::vector<Component*>>(comp);
			for (auto& comp2 : vComp)
				comp2->Update();
		}

	}

}

void GameObject::CleanUp()
{
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

	delete[] components; 
}


bool GameObject::AddComponent(Component* comp)
{
	if (comp != nullptr)
	{
		// Check if the component can be added to a GameObject 
		if (std::get<0>(uniquenessMap.at(comp->type)) == false)
			return false; 

		// Check if the component is unique 
		if (std::get<2>(uniquenessMap.at(comp->type)) == true)
		{
			// Check if there already exists a component of that type
			if (std::get<Component*>(components[comp->type]) != nullptr)
				std::get<Component*>(components[comp->type])->CleanUp();

			std::get<Component*>(components[comp->type]) = comp;
		}
		else // if it is not unique, push it to that component type list 
			std::get<std::vector<Component*>>(components[comp->type]).push_back(comp); 
	
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

		// Check if the component is unique 
		if (std::get<2>(uniquenessMap.at(comp->type)) == true)
		{
			// Check if there already exists a component of that type
			if (std::get<Component*>(mesh->components[comp->type]) != nullptr)
				std::get<Component*>(mesh->components[comp->type])->CleanUp();

			std::get<Component*>(mesh->components[comp->type]) = comp;
		}
		else // if it is not unique, push it to that component type list 
			std::get<std::vector<Component*>>(mesh->components[comp->type]).push_back(comp);

		comp->parent = this;
		comp->Enable();

		return true; 
	}

	return false; 
}