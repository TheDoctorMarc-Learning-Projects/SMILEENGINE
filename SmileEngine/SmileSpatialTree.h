#pragma once

#include "SmileModule.h"
#include "ComponentMesh.h"
#include "BoundingData.h"

class OctreeNode
{
public: 
	OctreeNode(OctreeNode* parentNode, uint i);
	//static OctreeNode root(); 
	~OctreeNode() {};

public: 
	bool Insert(GameObject*);
	void Split(); 
	OctreeNode* GetChildren() const { return childNodes[0]; };
private: 
	uint depth = 0; 
	bounding::BoundingBox AABB; 
	std::vector<GameObject*> insideObjs; 
	OctreeNode* childNodes[8] = { nullptr };
	OctreeNode* parentNode; 
};

class SmileSpatialTree : public SmileModule
{
	SmileSpatialTree(SmileApp* app, bool start_enabled = true);
	~SmileSpatialTree();

	bool CreateOctree(float3 fromTo[2], uint depth = 10);

	bool InsertToTree(GameObject* obj);

private: 
	OctreeNode* root; 
	uint maxDepth = 10; 
};

typedef SmileSpatialTree Octree; 