/*#ifndef __PhysBody3D_H__
#define __PhysBody3D_H__

#include "p2List.h"
#include "SmileSetup.h"

class btRigidBody;
class Module;
class vec3; 
class btVector3; 
class btQuaternion;

// =================================================
struct PhysBody3D
{
	friend class ModulePhysics3D;
public:
	PhysBody3D(btRigidBody* body);
	~PhysBody3D();

	void Push(float x, float y, float z);
	void GetTransform(float* matrix) const;
	void SetTransform(const float* matrix) const;
	void SetPos(float x, float y, float z);
	void Set_Speed(btVector3);
	void Set_Orientation(float angle, vec3 axis); 
	btRigidBody* Get_Rigid_Body(); 
	//void SetBodyMass(float mass = 1.0f);
	void SetStatic(bool DBG_NEWCondition);
	bool isStatic();
	void SetEuler(float yaw, float roll);
	const btQuaternion* GetRotQuat() const;

	const vec3 GetPos() const;


	

private:
	btRigidBody* body = nullptr;
	bool static_state = false;

public:
	p2List<Module*> collision_listeners;
	bool is_sensor = false;
	bool _first_of_a_pair = NULL; 
};

#endif // __PhysBody3D_H__*/