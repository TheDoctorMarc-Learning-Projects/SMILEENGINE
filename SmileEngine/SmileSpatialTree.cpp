#include "SmileSpatialTree.h"
#include "Glew/include/GL/glew.h" 
#include "SmileApp.h"
#include "SmileScene.h"

SmileSpatialTree::SmileSpatialTree(SmileApp* app, bool start_enabled) : SmileModule(app, start_enabled){}
SmileSpatialTree::~SmileSpatialTree() {

	// TODO: how to release everything? :o 
}

void SmileSpatialTree::CreateOctree(float3 fromTo[2], uint depth, uint maxNodeObjects)
{
	MAX_NODE_OBJECTS = maxNodeObjects; 
	MAX_DEPTH = depth; 

	CreateRoot(fromTo);
}

void SmileSpatialTree::CreateRoot(float3 fromTo[2])
{
	// the root is to be created once
	static bool once = [this, fromTo]()
	{
		root = DBG_NEW OctreeNode();
		root->SetupAABB(math::AABB(fromTo[0], fromTo[1]));  
		ComputeObjectTree(); 

		return true; 
	} ();

}

void SmileSpatialTree::ComputeObjectTree()
{
	auto gameObjects = App->scene_intro->rootObj->GetChildrenRecursive(); 

	for (auto& obj : gameObjects)
		root->InsertObject(obj);
}

// ----------------------------------------------------------------- [OctreeNode]
OctreeNode::OctreeNode(OctreeNode* parentNode, uint i)
{
	this->parentNode = parentNode; 
	this->depth = parentNode->depth + 1; 

	// front Z face -> 0,1,2,3 and back Z face -> 4,5,6,7 counter clock-wise, starting from bottom left 
	float3 min, max; 
	min = max = float3(0, 0, 0);

	min.x = ((i == 0) || (i == 3) || (i == 4) || (i == 7)) ? parentNode->AABB.MinX() : (parentNode->AABB.MaxX() - parentNode->AABB.HalfSize().x); 
	max.x = ((i == 0) || (i == 3) || (i == 4) || (i == 7)) ? (parentNode->AABB.MinX() + parentNode->AABB.HalfSize().x) : parentNode->AABB.MaxX();

	min.y = ((i == 0) || (i == 1) || (i == 4) || (i == 5)) ? parentNode->AABB.MinY() : (parentNode->AABB.MaxY() - parentNode->AABB.HalfSize().y);
	max.y = ((i == 0) || (i == 1) || (i == 4) || (i == 5)) ? (parentNode->AABB.MinY() + parentNode->AABB.HalfSize().y) : parentNode->AABB.MaxY();

	min.z = (i > 3) ? parentNode->AABB.MinZ() : (parentNode->AABB.MaxZ() - parentNode->AABB.HalfSize().z);
	max.z = (i > 3) ? (parentNode->AABB.MinZ() + parentNode->AABB.HalfSize().z) : parentNode->AABB.MaxZ();
	
	this->AABB = math::AABB(min, max); 
}

inline static bool IsLeaf(OctreeNode* node) { return node->GetChildrenPointer() == nullptr; };

void OctreeNode::InsertObject(GameObject* obj)
{
	// if the Node has the maximum objects, but splitting means exceeding the max tree depth, rather keep the object for myself
	if (insideObjs.size() == MAX_NODE_OBJECTS && depth == MAX_DEPTH)
	{
		insideObjs.push_back(obj);
		return; 
	}
	
	// If the Node has less than the maximum objects and it has no children, push it directly
	if (insideObjs.size() < MAX_NODE_OBJECTS && IsLeaf(this))
	{
		insideObjs.push_back(obj);
	}
		
	else // If the Node has the maximum objects, if and only if splitting means exceeding the max tree depth
	{
		bool reArrange = false; 
		// Split if no children and max depth not reached yet
		if (IsLeaf(this) && depth < MAX_DEPTH)
		{
			Split();
			reArrange = true; 
		}
			
		// push the object, then spread it through chlildren if splitted
		insideObjs.push_back(obj);

		if(reArrange)
			RearrangeObjectsInChildren();

	}

}

void OctreeNode::Split()
{
	for (int i = 0; i < 8; ++i)
		childNodes[i] = DBG_NEW OctreeNode(this, i);
}

void OctreeNode::RearrangeObjectsInChildren()
{
	for(std::vector<GameObject*>::iterator obj = insideObjs.begin(); obj != insideObjs.end();)
	{
		GameObject* capture = (*obj); 

		std::array<bool, 8> intersections; 
		std::fill(std::begin(intersections), std::end(intersections), false);

		for (int i = 0; i < 8; ++i)
			if (childNodes[i]->AABB.Intersects((*obj)->GetBoundingData().AABB))
				intersections.at(i) = true; 

		// if the object intersects with all 8 child nodes, it'd be a waste to push it to all 8 nodes 
		if (std::all_of(std::begin(intersections), std::end(intersections), [](bool value) { return value; }))
		{
			++obj; 
		}
			
		else // otherwise erase it from the Node, and push it to all the intersecting children
		{
			obj = insideObjs.erase(obj);

			for (int i = 0; i < 8; ++i)
				if (intersections.at(i) == true)
					childNodes[i]->InsertObject(capture);
		}
		

	}
}

void OctreeNode::Debug()
{
	if(insideObjs.size() == 0)
		glColor3f(1.0f, 0.0f, 0.0f);
	else
		glColor3f(0.0f, 1.0f, 0.0f);

	glPointSize(10);
	glBegin(GL_POINTS);
	float3 pointsArray[8]; 
	this->AABB.GetCornerPoints(pointsArray);
	for (int i = 0; i < 8; ++i)
		glVertex3f((GLfloat)pointsArray[i].x, (GLfloat)pointsArray[i].y, (GLfloat)pointsArray[i].z);

	glEnd();
	glPointSize(1); 
	glColor3f(1.0f, 1.0f, 1.0f);


	// children
	if (IsLeaf(this) == false)
		for (auto& node : childNodes)
			node->Debug(); 
		
}
