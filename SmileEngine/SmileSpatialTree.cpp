#include "SmileSpatialTree.h"
#include "Glew/include/GL/glew.h" 

SmileSpatialTree::SmileSpatialTree(SmileApp* app, bool start_enabled) : SmileModule(app, start_enabled){}
SmileSpatialTree::~SmileSpatialTree() {

	// TODO: how to release everything? :o 
}

bool SmileSpatialTree::CreateOctree(float3 fromTo[2], uint depth, uint maxNodeObjects)
{
	MAX_NODE_OBJECTS = maxNodeObjects; 
	MAX_DEPTH = depth; 

	CreateRoot(fromTo);

	return true; 
}

bool SmileSpatialTree::InsertToTree(GameObject* obj)
{
	if (obj)
		if (root->Insert(obj))
			return true; 
	return false; 
}


void SmileSpatialTree::CreateRoot(float3 fromTo[2])
{
	// the root is to be created once
	[this, fromTo]()
	{
		root = DBG_NEW OctreeNode();
		root->SetupAABB(math::AABB(fromTo[0], fromTo[1])); 

		// just for testing, it should not split yet: 
		root->Split(); 
	} ();

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

inline static bool IsLeaf(OctreeNode* node) { return node->GetChildren() == nullptr; };

bool OctreeNode::Insert(GameObject* obj)
{
	if (IsLeaf(this) && insideObjs.size() < MAX_NODE_OBJECTS)
	{
		insideObjs.push_back(obj);
	}
	else
	{
		if(IsLeaf(this))
			Split();
		
		//RearrangeChildren(); 
	}

	/*if (AABB.Intersects(obj->GetMesh()->GetMeshData()->Getmaabb())) // todo: calculate a global aabb for the object encompassing everything inside
	{
		insideObjs.push_back(obj); 
		Split(); 

		return true; 
	}
	*/
	return false; 
}

void OctreeNode::Split()
{
	for (int i = 0; i < 8; ++i)
		childNodes[i] = DBG_NEW OctreeNode(this, i);
}

void OctreeNode::Debug()
{
	glColor3f(0.0f, 0.0f, 1.0f);
	glPointSize(10);
	glBegin(GL_POINTS);
	float3 pointsArray[8]; 
	this->AABB.GetCornerPoints(pointsArray);
	for (int i = 0; i < 8; ++i)
	{
	    glColor3f(0.0f, 0.0f, 1.0f);
		glVertex3f((GLfloat)pointsArray[i].x, (GLfloat)pointsArray[i].y, (GLfloat)pointsArray[i].z);
	}
	glEnd();
	glPointSize(1); 
	glColor3f(1.0f, 1.0f, 1.0f);


	// children
	if (IsLeaf(this) == false)
		for (auto& node : childNodes)
			node->Debug(); 
		
}
