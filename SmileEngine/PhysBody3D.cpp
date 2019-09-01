#include "PhysBody3D.h"
#include "glmath.h"
#include <math.h>
#include "Bullet/include/btBulletDynamicsCommon.h"

// =================================================
PhysBody3D::PhysBody3D(btRigidBody* body) : body(body)
{
	body->setUserPointer(this);
}

// ---------------------------------------------------------
PhysBody3D::~PhysBody3D()
{
	delete body;
}

// ---------------------------------------------------------
void PhysBody3D::Push(float x, float y, float z)
{
	// force body activation before impulse
	body->activate(true);
	body->applyCentralImpulse(btVector3(x, y, z));
}

// ---------------------------------------------------------
void PhysBody3D::GetTransform(float* matrix) const
{
	if(body != NULL && matrix != NULL)
	{
		body->getWorldTransform().getOpenGLMatrix(matrix);
	}
}

// ---------------------------------------------------------
void PhysBody3D::SetTransform(const float* matrix) const
{
	if(body != NULL && matrix != NULL)
	{
		btTransform t;
		t.setFromOpenGLMatrix(matrix);
		body->setWorldTransform(t);
	}
}

// ---------------------------------------------------------
void PhysBody3D::SetPos(float x, float y, float z)
{
	btTransform t = body->getWorldTransform();
	t.setOrigin(btVector3(x, y, z));
	body->setWorldTransform(t);
}



const vec3 PhysBody3D::GetPos() const {

	vec3 def_pos;
	btTransform t = body->getWorldTransform();
	btVector3 pos = t.getOrigin();
	def_pos.Set(pos.getX(), pos.getY(), pos.getZ()); 
	return def_pos;
}



void PhysBody3D::Set_Speed(btVector3 speed) {
	body->setLinearVelocity(speed);
}



void PhysBody3D::Set_Orientation(float angle, vec3 axis) {
	btTransform t = body->getWorldTransform();

	// angle = angle * 180 / _PI; 
	
	btScalar x = axis.x * sin(angle / 2); 
	btScalar y = axis.y * sin(angle / 2);
	btScalar z = axis.z * sin(angle / 2);
	btScalar q_real_part = cos(angle / 2); 

	btQuaternion q(x, y, z, q_real_part); 
	q.normalize(); 
	t.setRotation(q); 
	body->setWorldTransform(t); 

}

btRigidBody* PhysBody3D::Get_Rigid_Body() {
	return body; 
}

void PhysBody3D::SetStatic(bool DBG_NEWCondition)
{
	/* Make the body static without changing the mass property */
	if (DBG_NEWCondition)
		body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_STATIC_OBJECT);
	else
	/* clear the static object flag */
		body->setCollisionFlags(body->getCollisionFlags() & ~btCollisionObject::CF_STATIC_OBJECT);

	// force body activation
	body->activate(true);
	// updates internal physbody state
	static_state = DBG_NEWCondition;
}

bool PhysBody3D::isStatic()
{
	return static_state;
}

void PhysBody3D::SetEuler(float yawAngle, float rollAngle)
{
	// set rigidbody rotation transform 
	btTransform tr;
	tr.setIdentity();
	btQuaternion quat;
	// yaw,pitch,roll
	quat.setEuler(yawAngle * _PI / 180, 0, rollAngle * _PI / 180);
	tr.setRotation(quat);

	body->setCenterOfMassTransform(tr);
}

const btQuaternion* PhysBody3D::GetRotQuat() const
{
	return &body->getCenterOfMassTransform().getRotation();
}
