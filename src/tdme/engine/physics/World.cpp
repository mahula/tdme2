// Generated from /tdme/src/tdme/engine/physics/World.java
#include <tdme/engine/physics/World.h>

#include <map>
#include <string>

#include <java/lang/Class.h>
#include <java/lang/Math.h>
#include <java/lang/Object.h>
#include <java/lang/String.h>
#include <java/lang/StringBuilder.h>
#include <java/util/Iterator.h>
#include <tdme/engine/Engine.h>
#include <tdme/engine/Entity.h>
#include <tdme/engine/Rotation.h>
#include <tdme/engine/Rotations.h>
#include <tdme/engine/Transformations.h>
#include <tdme/engine/physics/CollisionResponse.h>
#include <tdme/engine/physics/ConstraintsEntity.h>
#include <tdme/engine/physics/ConstraintsSolver.h>
#include <tdme/engine/physics/PhysicsPartition.h>
#include <tdme/engine/physics/PhysicsPartitionOctTree.h>
#include <tdme/engine/physics/RigidBody.h>
#include <tdme/engine/physics/World_1.h>
#include <tdme/engine/physics/World_2.h>
#include <tdme/engine/primitives/BoundingBox.h>
#include <tdme/engine/primitives/BoundingVolume.h>
#include <tdme/engine/primitives/LineSegment.h>
#include <tdme/engine/primitives/OrientedBoundingBox.h>
#include <tdme/math/MathTools.h>
#include <tdme/math/Matrix4x4.h>
#include <tdme/math/Quaternion.h>
#include <tdme/math/Vector3.h>
#include <tdme/utils/ArrayListIteratorMultiple.h>
#include <tdme/utils/Pool.h>
#include <tdme/utils/_Console.h>
#include <Array.h>

using std::map;
using std::wstring;
using std::to_wstring;

using tdme::engine::physics::World;
using java::lang::Class;
using java::lang::Math;
using java::lang::Object;
using java::lang::String;
using java::lang::StringBuilder;
using java::util::Iterator;
using tdme::engine::Engine;
using tdme::engine::Entity;
using tdme::engine::Rotation;
using tdme::engine::Rotations;
using tdme::engine::Transformations;
using tdme::engine::physics::CollisionResponse;
using tdme::engine::physics::ConstraintsEntity;
using tdme::engine::physics::ConstraintsSolver;
using tdme::engine::physics::PhysicsPartition;
using tdme::engine::physics::PhysicsPartitionOctTree;
using tdme::engine::physics::RigidBody;
using tdme::engine::physics::World_1;
using tdme::engine::physics::World_2;
using tdme::engine::primitives::BoundingBox;
using tdme::engine::primitives::BoundingVolume;
using tdme::engine::primitives::LineSegment;
using tdme::engine::primitives::OrientedBoundingBox;
using tdme::math::MathTools;
using tdme::math::Matrix4x4;
using tdme::math::Quaternion;
using tdme::math::Vector3;
using tdme::utils::ArrayListIteratorMultiple;
using tdme::utils::Pool;
using tdme::utils::_Console;

template<typename T, typename U>
static T java_cast(U* u)
{
    if (!u) return static_cast<T>(nullptr);
    auto t = dynamic_cast<T>(u);
    return t;
}

World::World(const ::default_init_tag&)
	: super(*static_cast< ::default_init_tag* >(0))
{
	clinit();
}

World::World() 
	: World(*static_cast< ::default_init_tag* >(0))
{
	ctor();
}

void World::init()
{
	partition = new PhysicsPartitionOctTree();
	collisionMovement = new Vector3();
	worldPosForce = new Vector3();
	gravityForce = new Vector3();
	collision = new CollisionResponse();
	constraintsSolver = nullptr;
	heightBoundingBox = new BoundingBox();
	heightOnPointCandidate = new Vector3();
	heightOnPointA = new Vector3();
	heightOnPointB = new Vector3();
	heightOnPointLineSegment = new LineSegment();
	sideVector = new Vector3(1.0f, 0.0f, 0.0f);
	upVector = new Vector3(0.0f, 1.0f, 0.0f);
	forwardVector = new Vector3(0.0f, 0.0f, 1.0f);
	heightPoint = new Vector3();
	heightPointDest = new Vector3();
}

void World::ctor()
{
	super::ctor();
	init();
}

void World::reset()
{
	rigidBodies.clear();
	partition->reset();
}

PhysicsPartition* World::getPartition()
{
	return partition;
}

void World::setPartition(PhysicsPartition* partition)
{
	this->partition = partition;
}

RigidBody* World::addRigidBody(String* id, bool enabled, int32_t typeId, Transformations* transformations, BoundingVolume* obv, float restitution, float friction, float mass, Matrix4x4* inertiaMatrix)
{
	auto rigidBody = new RigidBody(this, rigidBodies.size(), id, enabled, typeId, obv, transformations, restitution, friction, mass, inertiaMatrix);
	rigidBodies.push_back(rigidBody);
	rigidBodiesDynamic.push_back(rigidBody);
	rigidBodiesById[id->getCPPWString()] = rigidBody;
	if (enabled == true)
		partition->addRigidBody(rigidBody);

	return rigidBody;
}

RigidBody* World::addStaticRigidBody(String* id, bool enabled, int32_t typeId, Transformations* transformations, BoundingVolume* obv, float friction)
{
	auto rigidBody = new RigidBody(this, rigidBodies.size(), id, enabled, typeId, obv, transformations, 0.0f, friction, 0.0f, RigidBody::computeInertiaMatrix(obv, 0.0f, 0.0f, 0.0f, 0.0f));
	rigidBodies.push_back(rigidBody);
	rigidBodiesById[id->getCPPWString()] = rigidBody;
	if (enabled == true)
		partition->addRigidBody(rigidBody);

	return rigidBody;
}

RigidBody* World::getRigidBody(String* id)
{
	auto rididBodyByIdIt = rigidBodiesById.find(id->getCPPWString());
	if (rididBodyByIdIt != rigidBodiesById.end()) {
		return rididBodyByIdIt->second;
	}
	return nullptr;
}

void World::update(float deltaTime)
{
	if (constraintsSolver == nullptr) {
		constraintsSolver = new ConstraintsSolver(rigidBodies);
	}
	for (auto i = 0; i < rigidBodies.size(); i++) {
		auto rigidBody = rigidBodies.at(i);
		if (rigidBody->enabled == false) {
			continue;
		}
		if (rigidBody->isStatic_ == true) {
			continue;
		}
		if (rigidBody->checkVelocityChange() == true) {
			rigidBody->awake(true);
		}
		if (rigidBody->isSleeping_ == true) {
			continue;
		}
		rigidBody->addForce(worldPosForce->set(rigidBody->getPosition())->setY(10000.0f), gravityForce->set(0.0f, -rigidBody->getMass() * MathTools::g, 0.0f));
	}
	auto collisionsTests = 0;
	rigidBodyTestedCollisions.clear();
	for (auto i = 0; i < rigidBodies.size(); i++) {
		auto rigidBody1 = rigidBodies.at(i);
		if (rigidBody1->enabled == false) {
			continue;
		}
		auto nearObjects = 0;
		for (auto _i = partition->getObjectsNearTo(rigidBody1->cbv)->iterator(); _i->hasNext(); ) {
			RigidBody* rigidBody2 = java_cast< RigidBody* >(_i->next());
			{
				if (rigidBody2->enabled == false) {
					continue;
				}
				if (rigidBody1->isStatic_ == true && rigidBody2->isStatic_ == true)
					continue;

				if (rigidBody1 == rigidBody2)
					continue;

				if (rigidBody1->isStatic_ == true && rigidBody2->isStatic_ == false && rigidBody2->isSleeping_ == true) {
					continue;
				}
				if (rigidBody2->isStatic_ == true && rigidBody1->isStatic_ == false && rigidBody1->isSleeping_ == true) {
					continue;
				}
				if (((rigidBody1->typeId & rigidBody2->collisionTypeIds) == rigidBody1->typeId) == false) {
					continue;
				}
				if (((rigidBody2->typeId & rigidBody1->collisionTypeIds) == rigidBody2->typeId) == false) {
					continue;
				}
				nearObjects++;
				wstring rigidBodyKey = to_wstring(rigidBody1->idx) + L"," + to_wstring(rigidBody2->idx);
				if (rigidBodyTestedCollisions.find(rigidBodyKey) != rigidBodyTestedCollisions.end()) {
					continue;
				}
				RigidBodyCollisionStruct rigidBodyCollisionStruct;
				rigidBodyCollisionStruct.rigidBody1Idx = rigidBody1->idx;
				rigidBodyCollisionStruct.rigidBody2Idx = rigidBody2->idx;
				rigidBodyTestedCollisions[rigidBodyKey] = rigidBodyCollisionStruct;
				collisionsTests++;
				collisionMovement->set(rigidBody1->movement);
				if (collisionMovement->computeLength() < MathTools::EPSILON) {
					collisionMovement->set(rigidBody2->movement);
					collisionMovement->scale(-1.0f);
				}
				if (rigidBody1->cbv->doesCollideWith(rigidBody2->cbv, collisionMovement, collision) == true && collision->hasPenetration() == true) {
					if (collision->getHitPointsCount() == 0)
						continue;

					rigidBodyCollisionsCurrentFrame[rigidBodyKey] = rigidBodyCollisionStruct;
					if (rigidBodyCollisionsLastFrame.find(rigidBodyKey) == rigidBodyCollisionsLastFrame.end()) {
						rigidBody1->fireOnCollisionBegin(rigidBody2, collision);
					}
					rigidBody1->fireOnCollision(rigidBody2, collision);
					if (rigidBody1->isStatic_ == false && rigidBody2->isStatic_ == false) {
						rigidBody1->awake(true);
						rigidBody2->awake(true);
					}
					constraintsSolver->allocateConstraintsEntity()->set(rigidBody1, rigidBody2, constraintsSolver->allocateCollision()->fromResponse(collision));
				}
			}
		}
	}

	for (auto it: rigidBodyCollisionsLastFrame) {
		RigidBodyCollisionStruct* rigidBodyCollisionStruct = &it.second;
		{
			wstring rigidBodyKey = to_wstring(rigidBodyCollisionStruct->rigidBody1Idx) + L"," + to_wstring(rigidBodyCollisionStruct->rigidBody2Idx);
			auto rigidBodyCollisionsCurrentFrameIt = rigidBodyCollisionsCurrentFrame.find(rigidBodyKey);
			if (rigidBodyCollisionsCurrentFrameIt != rigidBodyCollisionsCurrentFrame.end()) continue;
		}

		{
			wstring rigidBodyKey = to_wstring(rigidBodyCollisionStruct->rigidBody2Idx) + L"," + to_wstring(rigidBodyCollisionStruct->rigidBody1Idx);
			auto rigidBodyCollisionsCurrentFrameIt = rigidBodyCollisionsCurrentFrame.find(rigidBodyKey);
			if (rigidBodyCollisionsCurrentFrameIt != rigidBodyCollisionsCurrentFrame.end()) continue;
		}

		auto rigidBody1 = rigidBodies.at(rigidBodyCollisionStruct->rigidBody1Idx);
		auto rigidBody2 = rigidBodies.at(rigidBodyCollisionStruct->rigidBody2Idx);
		rigidBody1->fireOnCollisionEnd(rigidBody2);
	}

	auto rigidBodyCollisionsTmp = rigidBodyCollisionsLastFrame;
	rigidBodyCollisionsLastFrame = rigidBodyCollisionsCurrentFrame;
	rigidBodyCollisionsCurrentFrame = rigidBodyCollisionsTmp;
	rigidBodyCollisionsCurrentFrame.clear();
	constraintsSolver->compute(deltaTime);
	constraintsSolver->updateAllBodies(deltaTime);
	constraintsSolver->reset();
	for (auto i = 0; i < rigidBodies.size(); i++) {
		auto rigidBody = rigidBodies.at(i);
		if (rigidBody->enabled == false) {
			continue;
		}
		if (rigidBody->isStatic_ == true || rigidBody->isSleeping_ == true) {
			continue;
		}
		auto rotations = rigidBody->transformations->getRotations();
		while (rotations->size() > 1) {
			rotations->remove(rotations->size() - 1);
		}
		while (rotations->size() < 1) {
			rotations->add(new Rotation());
		}
		rotations->get(0)->fromQuaternion(rigidBody->orientation);
		(*rotations->get(0)->getAxix()->getArray())[1] *= -1.0f;
		auto transformations = rigidBody->transformations;
		transformations->getTranslation()->set(rigidBody->position);
		transformations->update();
		rigidBody->cbv->fromBoundingVolumeWithTransformations(rigidBody->obv, transformations);
		partition->updateRigidBody(rigidBody);
	}
}

void World::synch(Engine* engine)
{
	for (auto i = 0; i < rigidBodies.size(); i++) {
		auto rigidBody = rigidBodies.at(i);
		if (rigidBody->isStatic_ == true)
			continue;

		if (rigidBody->isSleeping_ == true)
			continue;

		auto engineEntity = engine->getEntity(rigidBody->id->getCPPWString());
		if (engineEntity == nullptr) {
			_Console::println(static_cast< Object* >(::java::lang::StringBuilder().append(u"World::entity '"_j)->append(rigidBody->id)
				->append(u"' not found"_j)->toString()));
			continue;
		}
		engineEntity->setEnabled(rigidBody->enabled);
		if (rigidBody->enabled == true) {
			engineEntity->fromTransformations(rigidBody->transformations);
		}
	}
}

Vector3* World::higher(Vector3* a, Vector3* b)
{
	clinit();
	return a->getY() > b->getY() ? a : b;
}

RigidBody* World::determineHeight(int32_t typeIds, float stepUpMax, Vector3* point, Vector3* dest)
{
	dest->set(point);
	auto pointXYZ = point->getArray();
	heightBoundingBox->getMin()->set((*pointXYZ)[0], -10000.0f, (*pointXYZ)[2]);
	heightBoundingBox->getMax()->set((*pointXYZ)[0], +10000.0f, (*pointXYZ)[2]);
	heightBoundingBox->update();
	heightOnPointCandidate->set((*pointXYZ)[0], 10000.0f, (*pointXYZ)[2]);
	auto height = -10000.0f;
	RigidBody* heightRigidBody = nullptr;
	for (auto _i = partition->getObjectsNearTo(static_cast< BoundingVolume* >(heightBoundingBox))->iterator(); _i->hasNext(); ) {
		RigidBody* rigidBody = java_cast< RigidBody* >(_i->next());
		{
			if (((rigidBody->typeId & typeIds) == rigidBody->typeId) == false)
				continue;

			auto cbv = rigidBody->cbv;
			if (dynamic_cast< BoundingBox* >(cbv) != nullptr) {
				if (heightOnPointLineSegment->doesBoundingBoxCollideWithLineSegment(java_cast< BoundingBox* >(cbv), heightBoundingBox->getMin(), heightBoundingBox->getMax(), heightOnPointA, heightOnPointB) == true) {
					auto heightOnPoint = higher(heightOnPointA, heightOnPointB);
					if (heightOnPoint->getY() >= height && heightOnPoint->getY() < (*pointXYZ)[1] + Math::max(0.1f, stepUpMax)) {
						height = heightOnPoint->getY();
						heightRigidBody = rigidBody;
					}
				}
			} else
			if (dynamic_cast< OrientedBoundingBox* >(cbv) != nullptr) {
				if (heightOnPointLineSegment->doesOrientedBoundingBoxCollideWithLineSegment(java_cast< OrientedBoundingBox* >(cbv), heightBoundingBox->getMin(), heightBoundingBox->getMax(), heightOnPointA, heightOnPointB) == true) {
					auto heightOnPoint = higher(heightOnPointA, heightOnPointB);
					if (heightOnPoint->getY() >= height && heightOnPoint->getY() < (*pointXYZ)[1] + Math::max(0.1f, stepUpMax)) {
						height = heightOnPoint->getY();
						heightRigidBody = rigidBody;
					}
				}
			} else {
				cbv->computeClosestPointOnBoundingVolume(heightOnPointCandidate, heightOnPointA);
				if (Math::abs(heightOnPointCandidate->getX() - heightOnPointA->getX()) < 0.1f && Math::abs(heightOnPointCandidate->getZ() - heightOnPointA->getZ()) < 0.1f && heightOnPointA->getY() >= height && heightOnPointA->getY() < (*pointXYZ)[1] + Math::max(0.1f, stepUpMax)) {
					height = heightOnPointA->getY();
					heightRigidBody = rigidBody;
				}
			}
		}
	}
	if (heightRigidBody == nullptr) {
		return nullptr;
	}
	dest->setY(height);
	return heightRigidBody;
}

RigidBody* World::determineHeight(int32_t typeIds, float stepUpMax, BoundingVolume* boundingVolume, Vector3* point, Vector3* dest)
{
	auto determinedHeight = -10000.0f;
	auto width = boundingVolume->computeDimensionOnAxis(sideVector);
	auto height = boundingVolume->computeDimensionOnAxis(upVector);
	auto depth = boundingVolume->computeDimensionOnAxis(forwardVector);
	float heightPointDestY;
	RigidBody* heightRigidBody = nullptr;
	RigidBody* rigidBody = nullptr;
	heightPoint->set(boundingVolume->getCenter());
	heightPoint->addY(-height / 2.0f);
	rigidBody = determineHeight(typeIds, stepUpMax, heightPoint, heightPointDest);
	if (rigidBody != nullptr) {
		heightPointDestY = heightPointDest->getY();
		if (heightPointDestY > determinedHeight) {
			heightRigidBody = rigidBody;
			determinedHeight = heightPointDestY;
		}
	}
	heightPoint->set(boundingVolume->getCenter());
	heightPoint->addX(-width / 2.0f);
	heightPoint->addY(-height / 2.0f);
	heightPoint->addZ(-depth / 2.0f);
	rigidBody = determineHeight(typeIds, stepUpMax, heightPoint, heightPointDest);
	if (rigidBody != nullptr) {
		heightPointDestY = heightPointDest->getY();
		if (heightPointDestY > determinedHeight) {
			heightRigidBody = rigidBody;
			determinedHeight = heightPointDestY;
		}
	}
	heightPoint->set(boundingVolume->getCenter());
	heightPoint->addX(-width / 2.0f);
	heightPoint->addY(-height / 2.0f);
	heightPoint->addZ(+depth / 2.0f);
	rigidBody = determineHeight(typeIds, stepUpMax, heightPoint, heightPointDest);
	if (rigidBody != nullptr) {
		heightPointDestY = heightPointDest->getY();
		if (heightPointDestY > determinedHeight) {
			heightRigidBody = rigidBody;
			determinedHeight = heightPointDestY;
		}
	}
	heightPoint->set(boundingVolume->getCenter());
	heightPoint->addX(+width / 2.0f);
	heightPoint->addY(-height / 2.0f);
	heightPoint->addZ(-depth / 2.0f);
	rigidBody = determineHeight(typeIds, stepUpMax, heightPoint, heightPointDest);
	if (rigidBody != nullptr) {
		heightPointDestY = heightPointDest->getY();
		if (heightPointDestY > determinedHeight) {
			heightRigidBody = rigidBody;
			determinedHeight = heightPointDestY;
		}
	}
	heightPoint->set(boundingVolume->getCenter());
	heightPoint->addX(+width / 2.0f);
	heightPoint->addY(-height / 2.0f);
	heightPoint->addZ(+depth / 2.0f);
	rigidBody = determineHeight(typeIds, stepUpMax, heightPoint, heightPointDest);
	if (rigidBody != nullptr) {
		heightPointDestY = heightPointDest->getY();
		if (heightPointDestY > determinedHeight) {
			heightRigidBody = rigidBody;
			determinedHeight = heightPointDestY;
		}
	}
	if (heightRigidBody == nullptr) {
		return nullptr;
	} else {
		dest->set(point);
		dest->setY(determinedHeight);
		return heightRigidBody;
	}
}

const vector<RigidBody*>& World::doesCollideWith(int32_t typeIds, BoundingVolume* boundingVolume)
{
	collidedRigidBodies.clear();
	for (auto _i = partition->getObjectsNearTo(boundingVolume)->iterator(); _i->hasNext(); ) {
		RigidBody* rigidBody = java_cast< RigidBody* >(_i->next());
		{
			if (((rigidBody->typeId & typeIds) == rigidBody->typeId) == false)
				continue;

			if (rigidBody->cbv->doesCollideWith(boundingVolume, nullptr, collision) == true && collision->hasPenetration() == true) {
				collidedRigidBodies.push_back(rigidBody);
			}
		}
	}
	return collidedRigidBodies;
}

World* World::clone()
{
	auto clonedWorld = new World();
	for (auto i = 0; i < rigidBodies.size(); i++) {
		auto rigidBody = rigidBodies.at(i);
		auto obv = rigidBody->obv == nullptr ? static_cast< BoundingVolume* >(nullptr) : rigidBody->obv->clone();
		RigidBody* clonedRigidBody = nullptr;
		if (rigidBody->isStatic_ == true) {
			clonedRigidBody = clonedWorld->addStaticRigidBody(rigidBody->id, rigidBody->enabled, rigidBody->typeId, rigidBody->transformations, obv, rigidBody->friction);
		} else {
			clonedRigidBody = clonedWorld->addRigidBody(rigidBody->id, rigidBody->enabled, rigidBody->typeId, rigidBody->transformations, obv, rigidBody->restitution, rigidBody->friction, rigidBody->mass, rigidBody->inverseInertia->clone());
		}
		synch(clonedRigidBody, clonedRigidBody);
	}
	return clonedWorld;
}

void World::synch(RigidBody* clonedRigidBody, RigidBody* rigidBody)
{
	clonedRigidBody->enabled = rigidBody->enabled;
	clonedRigidBody->isSleeping_ = rigidBody->isSleeping_;
	clonedRigidBody->collisionTypeIds = rigidBody->collisionTypeIds;
	clonedRigidBody->cbv->fromBoundingVolume(rigidBody->cbv);
	clonedRigidBody->isStatic_ = rigidBody->isStatic_;
	clonedRigidBody->mass = rigidBody->mass;
	clonedRigidBody->inverseMass = rigidBody->inverseMass;
	clonedRigidBody->force->set(rigidBody->force);
	clonedRigidBody->torque->set(rigidBody->torque);
	clonedRigidBody->orientation->set(rigidBody->orientation);
	clonedRigidBody->angularVelocity->set(rigidBody->angularVelocity);
	clonedRigidBody->linearVelocity->set(rigidBody->linearVelocity);
	clonedRigidBody->angularVelocityLast->set(rigidBody->angularVelocityLast);
	clonedRigidBody->movement->set(rigidBody->movement);
	clonedRigidBody->position->set(rigidBody->position);
	clonedRigidBody->worldInverseInertia->set(rigidBody->worldInverseInertia);
	clonedRigidBody->transformations->fromTransformations(rigidBody->transformations);
}

void World::synch(World* world)
{
	for (auto i = 0; i < rigidBodiesDynamic.size(); i++) {
		auto rigidBody = rigidBodiesDynamic.at(i);
		auto clonedRigidBody = world->getRigidBody(rigidBody->id);
		if (clonedRigidBody == nullptr) {
			_Console::println(static_cast< Object* >(::java::lang::StringBuilder().append(u"Cloned world::entity '"_j)->append(rigidBody->id)
				->append(u"' not found"_j)->toString()));
			continue;
		}
		synch(clonedRigidBody, rigidBody);
		if (clonedRigidBody->enabled == true) {
			world->partition->updateRigidBody(clonedRigidBody);
		} else {
			world->partition->removeRigidBody(clonedRigidBody);
		}
	}
}

String* World::toString()
{
	StringBuilder result = ::java::lang::StringBuilder();
	result.append(u"World [rigidBodies="_j);
	for (int i = 0; i < rigidBodies.size(); i++) {
		if (i != 0) result.append(u","_j);
		result.append(rigidBodies.at(i)->toString());
	}
	result.append(u"]"_j);
	return result.toString();
}

extern java::lang::Class* class_(const char16_t* c, int n);

java::lang::Class* World::class_()
{
    static ::java::lang::Class* c = ::class_(u"tdme.engine.physics.World", 25);
    return c;
}

java::lang::Class* World::getClass0()
{
	return class_();
}

