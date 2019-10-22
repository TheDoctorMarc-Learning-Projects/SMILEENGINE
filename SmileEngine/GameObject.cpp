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
	components[TRANSFORM] = (Component*)NULL; // one
	components[MESH] = std::vector<Component*>(); // multiple
	components[MATERIAL] = (Component*)NULL;  // placeholder
	components[LIGHT] = std::vector<Component*>(); // multiple
}

void GameObject::Start()
{
	ComputeInitialData();
	dynamic_cast<ComponentTransform*>(std::get<Component*>(components[TRANSFORM]))->SetLocalMatrix(math::float3x3::identity); 
	Enable(); 
}

// this is used just ONCE to setup the transform at the avg point of all the meshes centers
void GameObject::ComputeInitialData()
{
	static auto lambda = [&]()
	{
		ComputeCenterAndSetupTransformThere();
		ComputeBoundingSphereRadius();

		return true; 
	}(); 
}

void GameObject::ComputeCenterAndSetupTransformThere() 
{
	static auto lambda = [&]()
	{
		std::vector<float> c_X, c_Y, c_Z;
		auto meshes = std::get<std::vector<Component*>>(GetComponent(MESH));

		// 1) accumulate the meshes centers in x,y and z vectors
		for (auto& m : meshes)
		{
			vec3 meshCenter = dynamic_cast<ComponentMesh*>(m)->GetMeshData()->GetMeshCenter();
			c_X.push_back(meshCenter.x);
			c_Y.push_back(meshCenter.y);
			c_Z.push_back(meshCenter.z);
		}

		// 2) summate all x's, y's and z's
		float tM = meshes.size();
		float3 center(0, 0, 0);

		for (auto& n : c_X)
			center.x += n;
		for (auto& n : c_Y)
			center.y += n;
		for (auto& n : c_Z)
			center.z += n;

		// 3) divide the summation by the number of meshes to find the average (X,Y,Z) point aka the GameObject center
		for (int i = 0; i <= 2; ++i)
			center[i] /= tM;

		// 4) setup the gameObject transform there, without updating any child 
		auto transf = dynamic_cast<ComponentTransform*>(std::get<Component*>(GetComponent(TRANSFORM)));
		transf->ChangePosition(center, false);

		return true;
	} ();

}

void GameObject::ComputeBoundingSphereRadius()
{

	// 1) each mesh has aabb aka min-max coordinates:
	// iterate them and find the global min-max for the gameObject
	static auto lambda = [&]()
	{
		auto meshes = std::get<std::vector<Component*>>(GetComponent(MESH));
		std::array<float, minMaxCoords::TOTAL_COORDS> objMinMax;
		int i = 0;
		for (auto& m : meshes)
		{
			// get the min-max of a particular mesh
			auto minMax = dynamic_cast<ComponentMesh*>(m)->GetMeshData()->GetMinMaxCoords();

			if (i == 0)
			{
				objMinMax = minMax;
				++i;
				continue;
			}

			// compare the mesh's min-max to the global min-max. Capture it if it is a min or a max. 

				// X min-max
			if (minMax[minMaxCoords::MIN_X] < objMinMax[minMaxCoords::MIN_X])
				objMinMax[minMaxCoords::MIN_X] = minMax[minMaxCoords::MIN_X];

			if (minMax[minMaxCoords::MAX_X] > objMinMax[minMaxCoords::MAX_X])
				objMinMax[minMaxCoords::MAX_X] = minMax[minMaxCoords::MAX_X];

			// Y min-max
			if (minMax[minMaxCoords::MIN_Y] < objMinMax[minMaxCoords::MIN_Y])
				objMinMax[minMaxCoords::MIN_Y] = minMax[minMaxCoords::MIN_Y];

			if (minMax[minMaxCoords::MAX_Y] > objMinMax[minMaxCoords::MAX_Y])
				objMinMax[minMaxCoords::MAX_Y] = minMax[minMaxCoords::MAX_Y];

			// Z min-max
			if (minMax[minMaxCoords::MIN_Z] < objMinMax[minMaxCoords::MIN_Z])
				objMinMax[minMaxCoords::MIN_Z] = minMax[minMaxCoords::MIN_Z];

			if (minMax[minMaxCoords::MAX_Z] > objMinMax[minMaxCoords::MAX_Z])
				objMinMax[minMaxCoords::MAX_Z] = minMax[minMaxCoords::MAX_Z];



			++i;
		}

		// 2) calculate the bounding sphere radius using the dist between two opposite min-max vertices
		// eg: the min(x,y,z) and the max(x,y,z)
		vec3 min_Vec(objMinMax[minMaxCoords::MIN_X], objMinMax[minMaxCoords::MIN_Y], objMinMax[minMaxCoords::MIN_Z]);
		vec3 max_Vec(objMinMax[minMaxCoords::MAX_X], objMinMax[minMaxCoords::MAX_Y], objMinMax[minMaxCoords::MAX_Z]);
		vec3 rad_Vec = (max_Vec - min_Vec) / 2;
		boundingSphereRadius = (double)sqrt(rad_Vec.x * rad_Vec.x + rad_Vec.y * rad_Vec.y + rad_Vec.y * rad_Vec.y);

		return true; 
	}(); 

}

void GameObject::Enable()
{
	
	// 1) Components
	// The variant "components" holds either a Component* if index = 0 or a std list of Components* if index = 1
	for (auto& comp : components)
	{
		if (comp.index() == 0)
		{
			Component* c = std::get<Component*>(comp);
			if (c)
			{
				if (c->active == false)
					c->Enable();
			}
			
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
			Component* c = std::get<Component*>(comp);
			if (c)
			{
				if (c->active == true)
					c->Disable();
			}
	 
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
			Component* c = std::get<Component*>(comp);
			if (c)
			{
				if (c->active == true)
					c->Enable();
			}
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
			Component* c = std::get<Component*>(comp);
			if (c)
			{
				c->CleanUp();
				RELEASE(c);
			}

	
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
