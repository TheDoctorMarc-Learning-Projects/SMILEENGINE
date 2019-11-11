#pragma once

#include "SmileModule.h"
#include "ComponentMesh.h"
#include "BoundingData.h"
#include "MathGeoLib/include/Geometry/AABB.h"

static uint MAX_NODE_OBJECTS = 10; 
static uint MAX_DEPTH = 4; 

// ----------------------------------------------------------------- [OctreeNode]
class OctreeNode
{
public: 
	OctreeNode() {}; // for root 
	OctreeNode(OctreeNode* parentNode, uint i); // for the rest of nodes
	~OctreeNode() {};

public: 
	void InsertObject(GameObject*);
	void Split(); 
	void RearrangeObjectsInChildren(); 
	OctreeNode* GetChildrenPointer() const { return childNodes[0]; };
	void SetupAABB(math::AABB aabb) { this->AABB = aabb; }; 
	void Debug();

private: 
	uint depth = 0; 
	math::AABB AABB; 
	std::vector<GameObject*> insideObjs; 
	OctreeNode* childNodes[8] = { nullptr };
	OctreeNode* parentNode; 

	friend class SmileSpatialTree; 
};

// ----------------------------------------------------------------- [Octree]
class SmileSpatialTree : public SmileModule
{
public: 
	SmileSpatialTree(SmileApp* app, bool start_enabled = true);
	~SmileSpatialTree();

	void CreateOctree(float3 fromTo[2], uint depth = 4, uint maxNodeObjects = 10);
	update_status Update(float dt) { /*root->Debug();*/ return update_status::UPDATE_CONTINUE; }; // to debug only
private: 
	void CreateRoot(float3 fromTo[2]); // for root 
	void ComputeObjectTree();
private: 
	OctreeNode* root = nullptr; 
 
	friend class OctreeNode; 
};

typedef SmileSpatialTree Octree; 