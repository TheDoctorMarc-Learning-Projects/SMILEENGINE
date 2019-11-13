#include "SmileSpatialTree.h"
#include "Glew/include/GL/glew.h" 
#include "SmileApp.h"
#include "SmileScene.h"
#include "imgui/imgui.h"
 

SmileSpatialTree::SmileSpatialTree(SmileApp* app, bool start_enabled) : SmileModule(app, start_enabled){}
SmileSpatialTree::~SmileSpatialTree() {}

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
		root = DBG_NEW OctreeNode(math::AABB(fromTo[0], fromTo[1]));
		ComputeObjectTree(App->scene_intro->rootObj); 

		return true; 
	} ();

}

void SmileSpatialTree::ComputeObjectTree(GameObject* obj)
{
	root->InsertObject(obj);
	auto children = obj->GetImmidiateChildren();
	for (auto& obj : children)
		ComputeObjectTree(obj);
}



bool SmileSpatialTree::CleanUp()
{
	root->CleanUp(); 
	RELEASE(root);

	return true; 
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
	// A) I have child nodes, then pass the object directly to them (conditions) 
	if (IsLeaf(this) == false)
	{
		if (SendObjectToChildren(obj) == false) 
			insideObjs.push_back(obj); 
	}
	else // B) I do not have child nodes right now
	{
		// if I have the maximum objects, but splitting means exceeding the max tree depth, rather keep the object for myself
		if (depth == MAX_DEPTH)
		{
			insideObjs.push_back(obj);
			return;
		}

		// If I have less than the maximum objects, push it directly
		if (insideObjs.size() < MAX_NODE_OBJECTS)
			insideObjs.push_back(obj);

		else // If I have the maximum objects, split and rearrange objects
		{
			Split();
			insideObjs.push_back(obj);
			RearrangeObjectsInChildren();

		}
	}

}

void OctreeNode::Split()
{
	for (int i = 0; i < 8; ++i)
		childNodes[i] = DBG_NEW OctreeNode(this, i);
}
 
// Push the object to children that can encompass it (check depth and object count) 
bool OctreeNode::SendObjectToChildren(GameObject* obj) 
{
	uint success = 0; 

	for (int i = 0; i < 8; ++i)
	{
		OctreeNode* childNode = childNodes[i]; 
		if (childNode->AABB.Intersects(obj->GetBoundingData().AABB))
		{
			success++; 
			childNode->InsertObject(obj); 
		}
	
	}
	
	// If the object has been pushed to any of the child nodes, return true
	if (success > 0)
		return true; 

	return false; 
}


// Takes all the objects and erases the ones that intersect with a child, while pushing it to the child's list
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
	
	glBegin(GL_LINES);
	float3 pointsArray[8]; 
	this->AABB.GetCornerPoints(pointsArray);
	
	// Direct Mode :( -> for the mom, TODO: optimized debug
	glVertex3f((GLfloat)pointsArray[0].x, (GLfloat)pointsArray[0].y, (GLfloat)pointsArray[0].z);
	glVertex3f((GLfloat)pointsArray[1].x, (GLfloat)pointsArray[1].y, (GLfloat)pointsArray[1].z);
	glVertex3f((GLfloat)pointsArray[1].x, (GLfloat)pointsArray[1].y, (GLfloat)pointsArray[1].z);
	glVertex3f((GLfloat)pointsArray[3].x, (GLfloat)pointsArray[3].y, (GLfloat)pointsArray[3].z);
	glVertex3f((GLfloat)pointsArray[3].x, (GLfloat)pointsArray[3].y, (GLfloat)pointsArray[3].z);
	glVertex3f((GLfloat)pointsArray[2].x, (GLfloat)pointsArray[2].y, (GLfloat)pointsArray[2].z);
	glVertex3f((GLfloat)pointsArray[2].x, (GLfloat)pointsArray[2].y, (GLfloat)pointsArray[2].z);
	glVertex3f((GLfloat)pointsArray[0].x, (GLfloat)pointsArray[0].y, (GLfloat)pointsArray[0].z);

	glVertex3f((GLfloat)pointsArray[3].x, (GLfloat)pointsArray[3].y, (GLfloat)pointsArray[3].z);
	glVertex3f((GLfloat)pointsArray[7].x, (GLfloat)pointsArray[7].y, (GLfloat)pointsArray[7].z);
	glVertex3f((GLfloat)pointsArray[7].x, (GLfloat)pointsArray[7].y, (GLfloat)pointsArray[7].z);
	glVertex3f((GLfloat)pointsArray[5].x, (GLfloat)pointsArray[5].y, (GLfloat)pointsArray[5].z);
	glVertex3f((GLfloat)pointsArray[5].x, (GLfloat)pointsArray[5].y, (GLfloat)pointsArray[5].z);
	glVertex3f((GLfloat)pointsArray[1].x, (GLfloat)pointsArray[1].y, (GLfloat)pointsArray[1].z);
	glVertex3f((GLfloat)pointsArray[1].x, (GLfloat)pointsArray[1].y, (GLfloat)pointsArray[1].z);
	glVertex3f((GLfloat)pointsArray[3].x, (GLfloat)pointsArray[3].y, (GLfloat)pointsArray[3].z);

	glVertex3f((GLfloat)pointsArray[7].x, (GLfloat)pointsArray[7].y, (GLfloat)pointsArray[7].z);
	glVertex3f((GLfloat)pointsArray[6].x, (GLfloat)pointsArray[6].y, (GLfloat)pointsArray[6].z);
	glVertex3f((GLfloat)pointsArray[6].x, (GLfloat)pointsArray[6].y, (GLfloat)pointsArray[6].z);
	glVertex3f((GLfloat)pointsArray[4].x, (GLfloat)pointsArray[4].y, (GLfloat)pointsArray[4].z);
	glVertex3f((GLfloat)pointsArray[4].x, (GLfloat)pointsArray[4].y, (GLfloat)pointsArray[4].z);
	glVertex3f((GLfloat)pointsArray[5].x, (GLfloat)pointsArray[5].y, (GLfloat)pointsArray[5].z);
	glVertex3f((GLfloat)pointsArray[5].x, (GLfloat)pointsArray[5].y, (GLfloat)pointsArray[5].z);
	glVertex3f((GLfloat)pointsArray[7].x, (GLfloat)pointsArray[7].y, (GLfloat)pointsArray[7].z);

	glVertex3f((GLfloat)pointsArray[2].x, (GLfloat)pointsArray[2].y, (GLfloat)pointsArray[2].z);
	glVertex3f((GLfloat)pointsArray[6].x, (GLfloat)pointsArray[6].y, (GLfloat)pointsArray[6].z);
	glVertex3f((GLfloat)pointsArray[6].x, (GLfloat)pointsArray[6].y, (GLfloat)pointsArray[6].z);
	glVertex3f((GLfloat)pointsArray[4].x, (GLfloat)pointsArray[4].y, (GLfloat)pointsArray[4].z);
	glVertex3f((GLfloat)pointsArray[4].x, (GLfloat)pointsArray[4].y, (GLfloat)pointsArray[4].z);
	glVertex3f((GLfloat)pointsArray[0].x, (GLfloat)pointsArray[0].y, (GLfloat)pointsArray[0].z);
	glVertex3f((GLfloat)pointsArray[0].x, (GLfloat)pointsArray[0].y, (GLfloat)pointsArray[0].z);
	glVertex3f((GLfloat)pointsArray[2].x, (GLfloat)pointsArray[2].y, (GLfloat)pointsArray[2].z);

	glVertex3f((GLfloat)pointsArray[3].x, (GLfloat)pointsArray[3].y, (GLfloat)pointsArray[3].z);
	glVertex3f((GLfloat)pointsArray[7].x, (GLfloat)pointsArray[7].y, (GLfloat)pointsArray[7].z);
	glVertex3f((GLfloat)pointsArray[7].x, (GLfloat)pointsArray[7].y, (GLfloat)pointsArray[7].z);
	glVertex3f((GLfloat)pointsArray[6].x, (GLfloat)pointsArray[6].y, (GLfloat)pointsArray[6].z);
	glVertex3f((GLfloat)pointsArray[6].x, (GLfloat)pointsArray[6].y, (GLfloat)pointsArray[6].z);
	glVertex3f((GLfloat)pointsArray[2].x, (GLfloat)pointsArray[2].y, (GLfloat)pointsArray[2].z);
	glVertex3f((GLfloat)pointsArray[2].x, (GLfloat)pointsArray[2].y, (GLfloat)pointsArray[2].z);
	glVertex3f((GLfloat)pointsArray[3].x, (GLfloat)pointsArray[3].y, (GLfloat)pointsArray[3].z);

	glVertex3f((GLfloat)pointsArray[1].x, (GLfloat)pointsArray[1].y, (GLfloat)pointsArray[1].z);
	glVertex3f((GLfloat)pointsArray[5].x, (GLfloat)pointsArray[5].y, (GLfloat)pointsArray[5].z);
	glVertex3f((GLfloat)pointsArray[5].x, (GLfloat)pointsArray[5].y, (GLfloat)pointsArray[5].z);
	glVertex3f((GLfloat)pointsArray[4].x, (GLfloat)pointsArray[4].y, (GLfloat)pointsArray[4].z);
	glVertex3f((GLfloat)pointsArray[4].x, (GLfloat)pointsArray[4].y, (GLfloat)pointsArray[4].z);
	glVertex3f((GLfloat)pointsArray[0].x, (GLfloat)pointsArray[0].y, (GLfloat)pointsArray[0].z);
	glVertex3f((GLfloat)pointsArray[0].x, (GLfloat)pointsArray[0].y, (GLfloat)pointsArray[0].z);
	glVertex3f((GLfloat)pointsArray[1].x, (GLfloat)pointsArray[1].y, (GLfloat)pointsArray[1].z);

	glEnd();
	glColor3f(1.0f, 1.0f, 1.0f);

	if(logged == false)
	{
		if (this != App->spatial_tree->root)
		{
			LOG("Octree Node here! Depth: %i /// Objects: %i /// Parent Depth: %i /// Parent Objects: %i",
				depth, insideObjs.size(), parentNode->depth, parentNode->insideObjs.size());

		}
		else
		{
			LOG("Octree Node here! Depth: %i /// Objects: %i /// No Parent, Root!!!",
				depth, insideObjs.size());
		}
		logged = true; 
	}
	
	
	// children
	if (IsLeaf(this) == false)
		for (auto& node : childNodes)
			node->Debug(); 
		
}

void OctreeNode::CleanUp()
{
	if (IsLeaf(this) == true)
		return; 

	for (auto& childNode : childNodes)
	{
		childNode->CleanUp();
		RELEASE(childNode);
	}
}