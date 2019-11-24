#pragma once

#include "SmileModule.h"
#include "ComponentMesh.h"
#include "ComponentCamera.h"
#include "MathGeoLib/include/Geometry/AABB.h"
#include <vector>

static uint MAX_NODE_OBJECTS = 10; 
static uint MAX_DEPTH = 8; 

class Frustrum;
// ----------------------------------------------------------------- [OctreeNode]
class OctreeNode
{
public: 
	OctreeNode(math::AABB aabb) { this->AABB = aabb; }; // for root 
	OctreeNode(OctreeNode* parentNode, uint i); // for the rest of nodes
	~OctreeNode(); 

public: 
	void DeleteObject(GameObject*);
	void InsertObject(GameObject*);
	void Debug();
	void GetInsideCount(uint& ret);
	void GetIfMaxObjects(uint& ret);

	// Checks a primitive intersects with a quadtree node, then checks the primitive intersects with the objects inside the node
	template<typename PRIMITIVE>
	void CollectCandidates(std::vector<GameObject*>& gameObjects, const PRIMITIVE& primitive)
	{
		if (primitive.Intersects(AABB))
		{
			for (auto& obj : insideObjs)
				if(primitive.Intersects(obj->GetBoundingData().OBB))
					gameObjects.push_back(obj);
				
			if (IsLeaf() == false)
			{
				for (uint i = 0; i < 8; ++i)
					childNodes[i]->CollectCandidates(gameObjects, primitive);
			}
			
		}
	}

	// used with frustrum in scene draw 
	template<typename PRIMITIVE>
	void CollectCandidatesA(std::vector<GameObject*>& gameObjects, const PRIMITIVE& primitive)
	{
		if (primitive.Intersects(AABB))
		{
			for (auto& obj : insideObjs)
			{
				if (obj->toDraw == false)  // must use a flag so the same obj is not pushed more than once (by other nodes)
				{
					gameObjects.push_back(obj); 
					obj->toDraw = true;
				}
			}
					

			if (IsLeaf() == false)
			{
				for (uint i = 0; i < 8; ++i)
					childNodes[i]->CollectCandidatesA(gameObjects, primitive);
			}

		}
	}


private: 
	inline bool IsLeaf() { return childNodes[0] == nullptr; };
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

	friend class SmileSpatialTree; 
};

// ----------------------------------------------------------------- [Octree]
class SmileSpatialTree : public SmileModule
{
public: 
	SmileSpatialTree(SmileApp* app, bool start_enabled = true);
	~SmileSpatialTree();

	void CreateOctree(math::AABB aabb, uint depth = MAX_DEPTH, uint maxNodeObjects = MAX_NODE_OBJECTS);
	update_status Update(float dt); 
	bool CleanUp(); 
	void OnStaticChange(GameObject* obj, bool isStatic); 
	uint GetNodeCount() const { return nodeCount; };
	uint GetInsideCount() const;
	uint GetNodesWithMaxObjects() const;
	uint GetMaxNodeObjects() const { return MAX_NODE_OBJECTS; };
	uint GetMaxNodeDepth() const { return MAX_DEPTH; };

	// ultimately checks an obb
	template<typename PRIMITIVE>
	void CollectCandidates(std::vector<GameObject*>& gameObjects, const PRIMITIVE& primitive)
	{
		root->CollectCandidates(gameObjects, primitive); 
	};

	// ultimately checks an aabb
	template<typename PRIMITIVE>
	void CollectCandidatesA(std::vector<GameObject*>& gameObjects, const PRIMITIVE& primitive)
	{
		root->CollectCandidatesA(gameObjects, primitive);
	};


private: 
	void CreateRoot(math::AABB aabb); // for root 
	void ComputeObjectTree(GameObject* obj);

private: 
	uint nodeCount = 0; // debug
	OctreeNode* root = nullptr; 
 
	friend class OctreeNode; 
};

typedef SmileSpatialTree Octree; 