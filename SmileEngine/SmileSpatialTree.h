#pragma once

#include "SmileModule.h"
#include "ComponentMesh.h"

#include "MathGeoLib/include/Geometry/AABB.h"

static uint MAX_NODE_OBJECTS = 10; 
static uint MAX_DEPTH = 4; 

// ----------------------------------------------------------------- [OctreeNode]
class OctreeNode
{
public: 
	OctreeNode(math::AABB aabb) { this->AABB = aabb; }; // for root 
	OctreeNode(OctreeNode* parentNode, uint i); // for the rest of nodes
	~OctreeNode() {};

public: 
	void InsertObject(GameObject*);
	OctreeNode* GetChildrenPointer() const { return childNodes[0]; };
	void Debug();

private: 
	void Split();
	bool SendObjectToChildren(GameObject* obj); 
	void RearrangeObjectsInChildren();
	void CleanUp(); 
private: 
	uint depth = 0; 
	math::AABB AABB; 
	std::vector<GameObject*> insideObjs; 
	OctreeNode* childNodes[8] = { nullptr };
	OctreeNode* parentNode; 

	// just debugging
	bool logged = false; 

	friend class SmileSpatialTree; 
};

// ----------------------------------------------------------------- [Octree]
class SmileSpatialTree : public SmileModule
{
public: 
	SmileSpatialTree(SmileApp* app, bool start_enabled = true);
	~SmileSpatialTree();

	void CreateOctree(float3 fromTo[2], uint depth = MAX_DEPTH, uint maxNodeObjects = MAX_NODE_OBJECTS);
	update_status Update(float dt) { root->Debug(); return update_status::UPDATE_CONTINUE; }; // to debug only
	bool CleanUp(); 
private: 
	void CreateRoot(float3 fromTo[2]); // for root 
	void ComputeObjectTree(GameObject* obj);
private: 
	OctreeNode* root = nullptr; 
 
	friend class OctreeNode; 
};

typedef SmileSpatialTree Octree; 