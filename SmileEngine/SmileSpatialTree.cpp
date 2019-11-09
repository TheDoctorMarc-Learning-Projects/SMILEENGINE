#include "SmileSpatialTree.h"

SmileSpatialTree::SmileSpatialTree(SmileApp* app, bool start_enabled) : SmileModule(app, start_enabled){}
SmileSpatialTree::~SmileSpatialTree() {}

bool SmileSpatialTree::CreateOctree(float3 fromTo[2], uint depth)
{
	maxDepth = depth; 
	//root = DBG_NEW OctreeNode::root();

	return true; 
}

bool SmileSpatialTree::InsertToTree(GameObject* obj)
{
	if (obj)
		if (root->Insert(obj))
			return true; 
	return false; 
}

// ----------------------------------------------------------------- [OctreeNode]
OctreeNode::OctreeNode(OctreeNode* parentNode, uint i)
{

}

//inline static bool IsLeaf(OctreeNode* node) { return node->GetChildren() == nullptr; };

bool OctreeNode::Insert(GameObject* obj)
{
	/*if ()
	{
		insideObjs.push_back(obj); 
		Split(); 

		return true; 
	}*/

	return false; 
}

void OctreeNode::Split()
{
	for (int i = 0; i < 8; ++i)
	{
		OctreeNode(this, i); 
	}
}