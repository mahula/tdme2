/*********************************************************************************
 * This source code is based on                                                  *
 * ReactPhysics3D physics library, http://www.reactphysics3d.com                 *
 * Copyright (c) 2010-2015 Daniel Chappuis                                       *
 *********************************************************************************
 *                                                                               *
 * This software is provided 'as-is', without any express or implied warranty.   *
 * In no event will the authors be held liable for any damages arising from the  *
 * use of this software.                                                         *
 *                                                                               *
 * Permission is granted to anyone to use this software for any purpose,         *
 * including commercial applications, and to alter it and redistribute it        *
 * freely, subject to the following restrictions:                                *
 *                                                                               *
 * 1. The origin of this software must not be misrepresented; you must not claim *
 *    that you wrote the original software. If you use this software in a        *
 *    product, an acknowledgment in the product documentation would be           *
 *    appreciated but is not required.                                           *
 *                                                                               *
 * 2. Altered source versions must be plainly marked as such, and must not be    *
 *    misrepresented as being the original software.                             *
 *                                                                               *
 * 3. This notice may not be removed or altered from any source distribution.    *
 *                                                                               *
 ********************************************************************************/

#pragma once

#include <algorithm>
#include <string>
#include <vector>

#include <tdme/tdme.h>
#include <tdme/engine/fwd-tdme.h>
#include <tdme/engine/Transformations.h>
#include <tdme/engine/physics/fwd-tdme.h>
#include <tdme/engine/primitives/fwd-tdme.h>
#include <tdme/math/fwd-tdme.h>
#include <tdme/math/Vector3.h>
#include <tdme/math/Matrix4x4.h>
#include <tdme/math/Quaternion.h>
#include <tdme/utils/fwd-tdme.h>
#include <tdme/engine/physics/CollisionListener.h>

using std::remove;
using std::vector;
using std::string;

using tdme::engine::Transformations;
using tdme::engine::physics::CollisionListener;
using tdme::engine::physics::CollisionResponse;
using tdme::engine::physics::World;
using tdme::engine::primitives::BoundingVolume;
using tdme::math::Matrix4x4;
using tdme::math::Quaternion;
using tdme::math::Vector3;

/** 
 * Rigid Body class
 * ported from "game physics - a practical introduction/ben kenwright"
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::engine::physics::RigidBody
{
	friend class ConstraintsSolver;
	friend class ContactCache;
	friend class ConstraintsEntity;
	friend class PhysicsPartitionOctTree;
	friend class World;
	friend class WorldCloned;

public:
	static constexpr int32_t TYPEIDS_ALL { 2147483647 };

	/**
	 * Inertia matrix
	 */
	struct InertiaMatrixSettings {
		bool noInertiaMatrix;
		float mass;
		float scaleX;
		float scaleY;
		float scaleZ;
	};

protected:
	static constexpr float LINEARVELOCITY_SLEEPTOLERANCE { 1.0f };
	static constexpr float ANGULARVELOCITY_SLEEPTOLERANCE { 2.0f };
	static constexpr int32_t SLEEPING_FRAMES { 300 };

	World* world {  };
	int32_t idx {  };
	string id {  };
	string rootId {  };
	int32_t typeId {  };
	int32_t collisionTypeIds {  };
	bool enabled {  };
	bool isStatic_ {  };
	bool isSleeping_ {  };

	int32_t sleepingFrameCount {  };

	Transformations transformations {  };
	BoundingVolume* obv {  };
	BoundingVolume* cbv {  };
	float friction {  };
	float restitution {  };
	float mass {  };
	float inverseMass {  };
	Vector3 movement {  };
	Vector3 position {  };
	Vector3 linearVelocity {  };
	Vector3 linearVelocityLast {  };
	Vector3 force {  };
	Quaternion orientation {  };
	Vector3 angularVelocity {  };
	Vector3 angularVelocityLast {  };
	Vector3 torque {  };
	InertiaMatrixSettings inverseInertiaSettings {  };
	Matrix4x4 orientationMatrix {  };
	Matrix4x4 worldInverseInertia {  };
	vector<CollisionListener*> collisionListeners {  };

	/**
	 * Wake up
	 */
	void awake(bool resetFrameCount);

	/**
	 * Compute world inverse inertia
	 */
	void computeWorldInverseInertiaMatrix();

	/**
	 * Updates this rigid body / integrates it
	 * @param delta time
	 */
	void update(float deltaTime);

	/**
	 * @return if velocity has been changed
	 */
	bool checkVelocityChange();

	/**
	 * Fire on collision
	 * @param other
	 * @param collision response
	 */
	void fireOnCollision(RigidBody* other, CollisionResponse* collisionResponse);

	/**
	 * Fire on collision begin
	 * @param other
	 * @param collision response
	 */
	void fireOnCollisionBegin(RigidBody* other, CollisionResponse* collisionResponse);

	/**
	 * Fire on collision end
	 * @param other
	 * @param collision response
	 */
	void fireOnCollisionEnd(RigidBody* other);

	/**
	 * Private constructor
	 * @param world
	 * @param id
	 * @param enabled
	 * @param type id
	 * @param restitution
	 * @param friction
	 * @param mass in kg
	 * @param inertia matrix settings
	 */
	RigidBody(World* world, const string& id, bool enabled, int32_t typeId, float restitution, float friction, float mass, const RigidBody::InertiaMatrixSettings& inverseInertiaSettings);

	/**
	 * Private constructor
	 * @param world
	 * @param id
	 * @param enabled
	 * @param type id
	 * @param obv
	 * @param transformations
	 * @param restitution
	 * @param friction
	 * @param mass in kg
	 * @param inertia matrix settings
	 */
	RigidBody(World* world, const string& id, bool enabled, int32_t typeId, BoundingVolume* obv, const Transformations& transformations, float restitution, float friction, float mass, const RigidBody::InertiaMatrixSettings& inverseInertiaSettings);

	/**
	 * Destructor
	 */
	virtual ~RigidBody();

	/**
	 * Dispose
	 */
	virtual void dispose();

	/**
	 * No rotation inertia matrix
	 * @param bv
	 * @param mass
	 * @return inertia matrix
	 */
	static Matrix4x4 _getNoRotationInertiaMatrix();

	/**
	 * Computes the inertia matrix
	 * @param bv
	 * @param mass
	 * @return inertia matrix
	 */
	static Matrix4x4 _computeInertiaMatrix(BoundingVolume* bv, float mass, float scaleXAxis, float scaleYAxis, float scaleZAxis);

public:

	/** 
	 * No rotation inertia matrix
	 * @param bv
	 * @param mass
	 * @return inertia matrix settings
	 */
	static InertiaMatrixSettings getNoRotationInertiaMatrix();

	/** 
	 * Computes the inertia matrix
	 * @param bv
	 * @param mass
	 * @return inertia matrix settings
	 */
	static InertiaMatrixSettings computeInertiaMatrix(BoundingVolume* bv, float mass, float scaleXAxis, float scaleYAxis, float scaleZAxis);

	/** 
	 * Set up index in rigid body array list
	 * @param idx
	 */
	void setIdx(int32_t idx);

	/** 
	 * @return index in rigid body array list
	 */
	int32_t getIdx();

	/** 
	 * @return id
	 */
	const string& getId();

	/** 
	 * @return root id
	 */
	const string& getRootId();

	/**
	 * Set root id
	 * @param root id
	 */
	void setRootId(const string& rootId);

	/**
	 * @return type id
	 */
	int32_t getTypeId();

	/**
	 * Set collision type
	 * @param type id
	 */
	void setTypeId(int typeId);

	/** 
	 * @return collision type ids bitmask
	 */
	int32_t getCollisionTypeIds();

	/** 
	 * Set up collision type ids
	 * @param collisionTypeIds
	 */
	void setCollisionTypeIds(int32_t collisionTypeIds);

	/** 
	 * @return if enabled
	 */
	bool isEnabled();

	/** 
	 * Set up if rigid body is enabled
	 * @param enabled
	 */
	void setEnabled(bool enabled);

	/** 
	 * @return object is static
	 */
	bool isStatic();

	/** 
	 * @return if sleeping
	 */
	bool isSleeping();

	/** 
	 * Set sleeping
	 * @param sleeping
	 */
	void setSleeping(bool sleeping);

	/**
	 * @return transformations
	 */
	const Transformations& getTransformations();

	/** 
	 * @return original bounding volume
	 */
	BoundingVolume* getBoudingVolume();

	/** 
	 * Set up bounding volume
	 * @param obv
	 */
	void setBoundingVolume(BoundingVolume* obv);

	/** 
	 * @return transformed bounding volume
	 */
	BoundingVolume* getBoundingVolumeTransformed();

	/** 
	 * @return position
	 */
	Vector3& getPosition();

	/** 
	 * @return last frame movement
	 */
	Vector3& getMovement();

	/** 
	 * @return friction
	 */
	float getFriction();

	/** 
	 * Set up friction
	 * @param friction
	 */
	void setFriction(float friction);

	/** 
	 * @return restitution / bouncyness
	 */
	float getRestitution();

	/** 
	 * Set up restitution
	 * @param restitution
	 */
	void setRestitution(float restitution);

	/** 
	 * @return mass
	 */
	float getMass();

	/** 
	 * Set up mass
	 * @param mass
	 */
	void setMass(float mass);

	/** 
	 * @return linear velocity
	 */
	Vector3& getLinearVelocity();

	/** 
	 * @return angular velocity
	 */
	Vector3& getAngularVelocity();

	/** 
	 * @return force
	 */
	Vector3& getForce();

	/** 
	 * Put rigid body to sleep
	 */
	void sleep();

	/** 
	 * Synchronizes this rigid body with transformations
	 * @param transformations
	 */
	void fromTransformations(const Transformations& transformations);

	/** 
	 * Add force
	 * @param position of world force
	 * @param direction magnitude
	 */
	void addForce(const Vector3& forceOrigin, const Vector3& force);

	/** 
	 * Add a collision listener to this rigid body
	 * @param listener
	 */
	void addCollisionListener(CollisionListener* listener);

	/** 
	 * Remove a collision listener to this rigid body
	 * @param listener
	 */
	void removeCollisionListener(CollisionListener* listener);

};
