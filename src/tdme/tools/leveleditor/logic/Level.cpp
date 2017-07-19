// Generated from /tdme/src/tdme/tools/leveleditor/logic/Level.java
#include <tdme/tools/leveleditor/logic/Level.h>

#include <vector>

#include <java/lang/Object.h>
#include <java/lang/String.h>
#include <java/lang/StringBuilder.h>
#include <tdme/engine/Engine.h>
#include <tdme/engine/Entity.h>
#include <tdme/engine/Light.h>
#include <tdme/engine/Object3D.h>
#include <tdme/engine/ObjectParticleSystemEntity.h>
#include <tdme/engine/PointsParticleSystemEntity.h>
#include <tdme/engine/Transformations.h>
#include <tdme/engine/model/Color4.h>
#include <tdme/engine/model/Color4Base.h>
#include <tdme/engine/model/Model.h>
#include <tdme/engine/physics/RigidBody.h>
#include <tdme/engine/physics/World.h>
#include <tdme/engine/primitives/OrientedBoundingBox.h>
#include <tdme/engine/primitives/Sphere.h>
#include <tdme/engine/subsystems/particlesystem/BoundingBoxParticleEmitter.h>
#include <tdme/engine/subsystems/particlesystem/CircleParticleEmitter.h>
#include <tdme/engine/subsystems/particlesystem/CircleParticleEmitterPlaneVelocity.h>
#include <tdme/engine/subsystems/particlesystem/PointParticleEmitter.h>
#include <tdme/engine/subsystems/particlesystem/SphereParticleEmitter.h>
#include <tdme/math/MathTools.h>
#include <tdme/math/Vector3.h>
#include <tdme/math/Vector4.h>
#include <tdme/tools/shared/model/LevelEditorEntity_EntityType.h>
#include <tdme/tools/shared/model/LevelEditorEntity.h>
#include <tdme/tools/shared/model/LevelEditorEntityBoundingVolume.h>
#include <tdme/tools/shared/model/LevelEditorEntityParticleSystem_BoundingBoxParticleEmitter.h>
#include <tdme/tools/shared/model/LevelEditorEntityParticleSystem_CircleParticleEmitter.h>
#include <tdme/tools/shared/model/LevelEditorEntityParticleSystem_CircleParticleEmitterPlaneVelocity.h>
#include <tdme/tools/shared/model/LevelEditorEntityParticleSystem_Emitter.h>
#include <tdme/tools/shared/model/LevelEditorEntityParticleSystem_ObjectParticleSystem.h>
#include <tdme/tools/shared/model/LevelEditorEntityParticleSystem_PointParticleEmitter.h>
#include <tdme/tools/shared/model/LevelEditorEntityParticleSystem_PointParticleSystem.h>
#include <tdme/tools/shared/model/LevelEditorEntityParticleSystem_SphereParticleEmitter.h>
#include <tdme/tools/shared/model/LevelEditorEntityParticleSystem_Type.h>
#include <tdme/tools/shared/model/LevelEditorEntityParticleSystem.h>
#include <tdme/tools/shared/model/LevelEditorLevel.h>
#include <tdme/tools/shared/model/LevelEditorLight.h>
#include <tdme/tools/shared/model/LevelEditorObject.h>
#include <tdme/tools/shared/model/ModelProperties.h>
#include <tdme/tools/shared/model/PropertyModelClass.h>
#include <tdme/utils/MutableString.h>
#include <tdme/utils/_Console.h>

using std::vector;

using tdme::tools::leveleditor::logic::Level;
using java::lang::Object;
using java::lang::String;
using java::lang::StringBuilder;
using tdme::engine::Engine;
using tdme::engine::Entity;
using tdme::engine::Light;
using tdme::engine::Object3D;
using tdme::engine::ObjectParticleSystemEntity;
using tdme::engine::PointsParticleSystemEntity;
using tdme::engine::Transformations;
using tdme::engine::model::Color4;
using tdme::engine::model::Color4Base;
using tdme::engine::model::Model;
using tdme::engine::physics::RigidBody;
using tdme::engine::physics::World;
using tdme::engine::primitives::OrientedBoundingBox;
using tdme::engine::primitives::Sphere;
using tdme::engine::subsystems::particlesystem::BoundingBoxParticleEmitter;
using tdme::engine::subsystems::particlesystem::CircleParticleEmitter;
using tdme::engine::subsystems::particlesystem::CircleParticleEmitterPlaneVelocity;
using tdme::engine::subsystems::particlesystem::PointParticleEmitter;
using tdme::engine::subsystems::particlesystem::SphereParticleEmitter;
using tdme::math::MathTools;
using tdme::math::Vector3;
using tdme::math::Vector4;
using tdme::tools::shared::model::LevelEditorEntity_EntityType;
using tdme::tools::shared::model::LevelEditorEntity;
using tdme::tools::shared::model::LevelEditorEntityBoundingVolume;
using tdme::tools::shared::model::LevelEditorEntityParticleSystem_BoundingBoxParticleEmitter;
using tdme::tools::shared::model::LevelEditorEntityParticleSystem_CircleParticleEmitter;
using tdme::tools::shared::model::LevelEditorEntityParticleSystem_CircleParticleEmitterPlaneVelocity;
using tdme::tools::shared::model::LevelEditorEntityParticleSystem_Emitter;
using tdme::tools::shared::model::LevelEditorEntityParticleSystem_ObjectParticleSystem;
using tdme::tools::shared::model::LevelEditorEntityParticleSystem_PointParticleEmitter;
using tdme::tools::shared::model::LevelEditorEntityParticleSystem_PointParticleSystem;
using tdme::tools::shared::model::LevelEditorEntityParticleSystem_SphereParticleEmitter;
using tdme::tools::shared::model::LevelEditorEntityParticleSystem_Type;
using tdme::tools::shared::model::LevelEditorEntityParticleSystem;
using tdme::tools::shared::model::LevelEditorLevel;
using tdme::tools::shared::model::LevelEditorLight;
using tdme::tools::shared::model::LevelEditorObject;
using tdme::tools::shared::model::ModelProperties;
using tdme::tools::shared::model::PropertyModelClass;
using tdme::utils::MutableString;
using tdme::utils::_Console;

template<typename T, typename U>
static T java_cast(U* u)
{
    if (!u) return static_cast<T>(nullptr);
    auto t = dynamic_cast<T>(u);
    return t;
}

Level::Level(const ::default_init_tag&)
	: super(*static_cast< ::default_init_tag* >(0))
{
	clinit();
}

Level::Level()
	: Level(*static_cast< ::default_init_tag* >(0))
{
	ctor();
}

constexpr int32_t Level::RIGIDBODY_TYPEID_STATIC;

constexpr int32_t Level::RIGIDBODY_TYPEID_PLAYER;

MutableString* Level::compareMutableString;

Transformations* Level::transformations;

void Level::setLight(Engine* engine, LevelEditorLevel* level, Vector3* translation)
{
	clinit();
	for (auto i = 0; i < level->getLightCount(); i++) {
		engine->getLightAt(i)->getAmbient()->set(static_cast< Color4Base* >(level->getLightAt(i)->getAmbient()));
		engine->getLightAt(i)->getDiffuse()->set(static_cast< Color4Base* >(level->getLightAt(i)->getDiffuse()));
		engine->getLightAt(i)->getSpecular()->set(static_cast< Color4Base* >(level->getLightAt(i)->getSpecular()));
		engine->getLightAt(i)->getPosition()->set(level->getLightAt(i)->getPosition());
		engine->getLightAt(i)->getSpotDirection()->set(level->getLightAt(i)->getSpotDirection());
		engine->getLightAt(i)->setSpotExponent(level->getLightAt(i)->getSpotExponent());
		engine->getLightAt(i)->setSpotCutOff(level->getLightAt(i)->getSpotCutOff());
		engine->getLightAt(i)->setConstantAttenuation(level->getLightAt(i)->getConstantAttenuation());
		engine->getLightAt(i)->setLinearAttenuation(level->getLightAt(i)->getLinearAttenuation());
		engine->getLightAt(i)->setQuadraticAttenuation(level->getLightAt(i)->getQuadraticAttenuation());
		engine->getLightAt(i)->setEnabled(level->getLightAt(i)->isEnabled());
		if (translation != nullptr) {
			engine->getLightAt(i)->getPosition()->setX(engine->getLightAt(i)->getPosition()->getX() + translation->getX());
			engine->getLightAt(i)->getPosition()->setY(engine->getLightAt(i)->getPosition()->getY() + translation->getY());
			engine->getLightAt(i)->getPosition()->setZ(engine->getLightAt(i)->getPosition()->getZ() + translation->getZ());
		}
	}
}

Entity* Level::createParticleSystem(LevelEditorEntityParticleSystem* particleSystem, String* id, bool enableDynamicShadows)
{
	clinit();
	ParticleEmitter* engineEmitter = nullptr;
	{
		auto v = particleSystem->getEmitter();
		if ((v == LevelEditorEntityParticleSystem_Emitter::NONE)) {
{
				return nullptr;
			}		}
		if ((v == LevelEditorEntityParticleSystem_Emitter::NONE) || (v == LevelEditorEntityParticleSystem_Emitter::POINT_PARTICLE_EMITTER)) {
{
				auto emitter = particleSystem->getPointParticleEmitter();
				engineEmitter = new PointParticleEmitter(emitter->getCount(), emitter->getLifeTime(), emitter->getLifeTimeRnd(), emitter->getMass(), emitter->getMassRnd(), emitter->getPosition(), emitter->getVelocity(), emitter->getVelocityRnd(), emitter->getColorStart(), emitter->getColorEnd());
				goto end_switch0;;
			}		}
		if ((v == LevelEditorEntityParticleSystem_Emitter::NONE) || (v == LevelEditorEntityParticleSystem_Emitter::POINT_PARTICLE_EMITTER) || (v == LevelEditorEntityParticleSystem_Emitter::BOUNDINGBOX_PARTICLE_EMITTER)) {
{
				auto emitter = particleSystem->getBoundingBoxParticleEmitters();
				engineEmitter = new BoundingBoxParticleEmitter(emitter->getCount(), emitter->getLifeTime(), emitter->getLifeTimeRnd(), emitter->getMass(), emitter->getMassRnd(), new OrientedBoundingBox(emitter->getObbCenter(), emitter->getObbAxis0(), emitter->getObbAxis1(), emitter->getObbAxis2(), emitter->getObbHalfextension()), emitter->getVelocity(), emitter->getVelocityRnd(), emitter->getColorStart(), emitter->getColorEnd());
				goto end_switch0;;
			}		}
		if ((v == LevelEditorEntityParticleSystem_Emitter::NONE) || (v == LevelEditorEntityParticleSystem_Emitter::POINT_PARTICLE_EMITTER) || (v == LevelEditorEntityParticleSystem_Emitter::BOUNDINGBOX_PARTICLE_EMITTER) || (v == LevelEditorEntityParticleSystem_Emitter::CIRCLE_PARTICLE_EMITTER)) {
{
				auto emitter = particleSystem->getCircleParticleEmitter();
				engineEmitter = new CircleParticleEmitter(emitter->getCount(), emitter->getLifeTime(), emitter->getLifeTimeRnd(), emitter->getAxis0(), emitter->getAxis1(), emitter->getCenter(), emitter->getRadius(), emitter->getMass(), emitter->getMassRnd(), emitter->getVelocity(), emitter->getVelocityRnd(), emitter->getColorStart(), emitter->getColorEnd());
				goto end_switch0;;
			}		}
		if ((v == LevelEditorEntityParticleSystem_Emitter::NONE) || (v == LevelEditorEntityParticleSystem_Emitter::POINT_PARTICLE_EMITTER) || (v == LevelEditorEntityParticleSystem_Emitter::BOUNDINGBOX_PARTICLE_EMITTER) || (v == LevelEditorEntityParticleSystem_Emitter::CIRCLE_PARTICLE_EMITTER) || (v == LevelEditorEntityParticleSystem_Emitter::CIRCLE_PARTICLE_EMITTER_PLANE_VELOCITY)) {
{
				auto emitter = particleSystem->getCircleParticleEmitterPlaneVelocity();
				engineEmitter = new CircleParticleEmitterPlaneVelocity(emitter->getCount(), emitter->getLifeTime(), emitter->getLifeTimeRnd(), emitter->getAxis0(), emitter->getAxis1(), emitter->getCenter(), emitter->getRadius(), emitter->getMass(), emitter->getMassRnd(), emitter->getVelocity(), emitter->getVelocityRnd(), emitter->getColorStart(), emitter->getColorEnd());
				goto end_switch0;;
			}		}
		if ((v == LevelEditorEntityParticleSystem_Emitter::NONE) || (v == LevelEditorEntityParticleSystem_Emitter::POINT_PARTICLE_EMITTER) || (v == LevelEditorEntityParticleSystem_Emitter::BOUNDINGBOX_PARTICLE_EMITTER) || (v == LevelEditorEntityParticleSystem_Emitter::CIRCLE_PARTICLE_EMITTER) || (v == LevelEditorEntityParticleSystem_Emitter::CIRCLE_PARTICLE_EMITTER_PLANE_VELOCITY) || (v == LevelEditorEntityParticleSystem_Emitter::SPHERE_PARTICLE_EMITTER)) {
{
				auto emitter = particleSystem->getSphereParticleEmitter();
				engineEmitter = new SphereParticleEmitter(emitter->getCount(), emitter->getLifeTime(), emitter->getLifeTimeRnd(), emitter->getMass(), emitter->getMassRnd(), new Sphere(emitter->getCenter(), emitter->getRadius()), emitter->getVelocity(), emitter->getVelocityRnd(), emitter->getColorStart(), emitter->getColorEnd());
				goto end_switch0;;
			}		}
		if (((v == LevelEditorEntityParticleSystem_Emitter::NONE) || (v == LevelEditorEntityParticleSystem_Emitter::POINT_PARTICLE_EMITTER) || (v == LevelEditorEntityParticleSystem_Emitter::BOUNDINGBOX_PARTICLE_EMITTER) || (v == LevelEditorEntityParticleSystem_Emitter::CIRCLE_PARTICLE_EMITTER) || (v == LevelEditorEntityParticleSystem_Emitter::CIRCLE_PARTICLE_EMITTER_PLANE_VELOCITY) || (v == LevelEditorEntityParticleSystem_Emitter::SPHERE_PARTICLE_EMITTER) || ((v != LevelEditorEntityParticleSystem_Emitter::NONE) && (v != LevelEditorEntityParticleSystem_Emitter::POINT_PARTICLE_EMITTER) && (v != LevelEditorEntityParticleSystem_Emitter::BOUNDINGBOX_PARTICLE_EMITTER) && (v != LevelEditorEntityParticleSystem_Emitter::CIRCLE_PARTICLE_EMITTER) && (v != LevelEditorEntityParticleSystem_Emitter::CIRCLE_PARTICLE_EMITTER_PLANE_VELOCITY) && (v != LevelEditorEntityParticleSystem_Emitter::SPHERE_PARTICLE_EMITTER)))) {
			_Console::println(static_cast< Object* >(::java::lang::StringBuilder().append(u"Level::createParticleSystem(): unknown particle system emitter '"_j)->append(static_cast< Object* >(particleSystem->getEmitter()))
				->append(u"'"_j)->toString()));
			return nullptr;
		}
end_switch0:;
	}

	{
		LevelEditorEntityParticleSystem_ObjectParticleSystem* objectParticleSystem;
		LevelEditorEntityParticleSystem_PointParticleSystem* pointParticleSystem;
		{
			auto v = particleSystem->getType();
			if ((v == LevelEditorEntityParticleSystem_Type::NONE)) {
				return nullptr;
			}
			if ((v == LevelEditorEntityParticleSystem_Type::OBJECT_PARTICLE_SYSTEM)) {
				auto objectParticleSystem = particleSystem->getObjectParticleSystem();
				if (objectParticleSystem->getModel() == nullptr)
					return nullptr;

				return new ObjectParticleSystemEntity(id, objectParticleSystem->getModel(), objectParticleSystem->getScale(), objectParticleSystem->isAutoEmit(), enableDynamicShadows, objectParticleSystem->getMaxCount(), engineEmitter);
			}
			if ((v == LevelEditorEntityParticleSystem_Type::POINT_PARTICLE_SYSTEM)) {
				auto pointParticleSystem = particleSystem->getPointParticleSystem();
				return new PointsParticleSystemEntity(id, false, engineEmitter, pointParticleSystem->getMaxPoints(), pointParticleSystem->isAutoEmit());
			}
			if ((((v != LevelEditorEntityParticleSystem_Type::NONE) && (v != LevelEditorEntityParticleSystem_Type::OBJECT_PARTICLE_SYSTEM) && (v != LevelEditorEntityParticleSystem_Type::POINT_PARTICLE_SYSTEM)))) {
				_Console::println(static_cast< Object* >(::java::lang::StringBuilder().append(u"Level::createParticleSystem(): unknown particle system type '"_j)->append(static_cast< Object* >(particleSystem->getType()))
					->append(u"'"_j)->toString()));
				return nullptr;
			}
end_switch1:;
		}
	}

}

void Level::addLevel(Engine* engine, LevelEditorLevel* level, bool addEmpties, bool addTrigger, bool dynamicShadowing, bool pickable, Vector3* translation)
{
	clinit();
	addLevel(engine, level, addEmpties, addTrigger, dynamicShadowing, pickable, translation, true);
}

void Level::addLevel(Engine* engine, LevelEditorLevel* level, bool addEmpties, bool addTrigger, bool dynamicShadowing, bool pickable, Vector3* translation, bool enable)
{
	clinit();
	for (auto i = 0; i < level->getObjectCount(); i++) {
		auto object = level->getObjectAt(i);
		auto properties = object->getTotalProperties();
		if (addEmpties == false && object->getEntity()->getType() == LevelEditorEntity_EntityType::EMPTY)
			continue;

		if (addTrigger == false && object->getEntity()->getType() == LevelEditorEntity_EntityType::TRIGGER)
			continue;

		Entity* entity = nullptr;
		if (object->getEntity()->getModel() != nullptr) {
			entity = new Object3D(object->getId(), object->getEntity()->getModel());
		} else if (object->getEntity()->getType() == LevelEditorEntity_EntityType::PARTICLESYSTEM) {
			entity = createParticleSystem(object->getEntity()->getParticleSystem(), object->getId(), false);
		}
		if (entity == nullptr)
			continue;

		entity->fromTransformations(object->getTransformations());
		if (translation != nullptr) {
			entity->getTranslation()->add(translation);
		}
		entity->setPickable(pickable);
		auto shadowingProperty = properties->getProperty(u"shadowing"_j);
		auto omitShadowing = shadowingProperty != nullptr && shadowingProperty->getValue()->equalsIgnoreCase(u"false"_j);
		entity->setDynamicShadowingEnabled(omitShadowing == true ? false : dynamicShadowing);
		if (object->getEntity()->getType() == LevelEditorEntity_EntityType::EMPTY) {
			entity->getScale()->set(MathTools::sign(entity->getScale()->getX()), MathTools::sign(entity->getScale()->getY()), MathTools::sign(entity->getScale()->getZ()));
		}
		entity->update();
		entity->setEnabled(enable);
		engine->addEntity(entity);
	}
}

void Level::addLevel(World* world, LevelEditorLevel* level, vector<RigidBody*>& rigidBodies, Vector3* translation)
{
	clinit();
	addLevel(world, level, rigidBodies, translation, true);
}

void Level::addLevel(World* world, LevelEditorLevel* level, vector<RigidBody*>& rigidBodies, Vector3* translation, bool enable)
{
	clinit();
	for (auto i = 0; i < level->getObjectCount(); i++) {
		auto object = level->getObjectAt(i);
		if (object->getEntity()->getType() == LevelEditorEntity_EntityType::EMPTY)
			continue;

		if (object->getEntity()->getType() == LevelEditorEntity_EntityType::TRIGGER)
			continue;

		if (object->getEntity()->getType() == LevelEditorEntity_EntityType::PARTICLESYSTEM)
			continue;

		for (auto j = 0; j < object->getEntity()->getBoundingVolumeCount(); j++) {
			auto entityBv = object->getEntity()->getBoundingVolumeAt(j);
			auto worldId = ::java::lang::StringBuilder().append(object->getId())->append(u".bv."_j)
				->append(j)->toString();
			auto transformations = new Transformations();
			transformations->fromTransformations(object->getTransformations());
			if (translation != nullptr) {
				transformations->getTranslation()->add(translation);
				transformations->update();
			}
			auto rigidBody = world->addStaticRigidBody(worldId, enable, RIGIDBODY_TYPEID_STATIC, transformations, entityBv->getBoundingVolume(), 1.0f);
			rigidBody->setCollisionTypeIds(RIGIDBODY_TYPEID_STATIC | RIGIDBODY_TYPEID_PLAYER);
			rigidBodies.push_back(rigidBody);
		}
	}
}

void Level::disableLevel(Engine* engine, LevelEditorLevel* level)
{
	clinit();
	for (auto i = 0; i < level->getObjectCount(); i++) {
		auto object = level->getObjectAt(i);
		auto entity = engine->getEntity(object->getId());
		if (entity == nullptr)
			continue;

		entity->setEnabled(false);
	}
}

void Level::disableLevel(World* world, vector<RigidBody*>& rigidBodies)
{
	clinit();
	for (auto rigidBody: rigidBodies) {
		rigidBody->setEnabled(false);
	}
}

void Level::enableLevel(Engine* engine, LevelEditorLevel* level, Vector3* translation)
{
	clinit();
	for (auto i = 0; i < level->getObjectCount(); i++) {
		auto object = level->getObjectAt(i);
		auto entity = engine->getEntity(object->getId());
		if (entity == nullptr)
			continue;

		entity->fromTransformations(object->getTransformations());
		if (translation != nullptr) {
			entity->getTranslation()->add(translation);
		}
		if (object->getEntity()->getType() == LevelEditorEntity_EntityType::EMPTY) {
			entity->getScale()->set(MathTools::sign(entity->getScale()->getX()), MathTools::sign(entity->getScale()->getY()), MathTools::sign(entity->getScale()->getZ()));
		}
		entity->update();
		entity->setEnabled(true);
	}
}

void Level::enableLevel(World* world, LevelEditorLevel* level, vector<RigidBody*>& rigidBodies, Vector3* translation)
{
	clinit();
	for (auto i = 0; i < level->getObjectCount(); i++) {
		auto object = level->getObjectAt(i);
		for (auto j = 0; j < object->getEntity()->getBoundingVolumeCount(); j++) {
			for (auto k = 0; k < rigidBodies.size(); k++) {
				auto rigidBody = rigidBodies.at(k);
				compareMutableString->set(object->getId());
				compareMutableString->append(u".bv."_j);
				compareMutableString->append(j);
				if (compareMutableString->equals(rigidBody->getId()) == false)
					continue;

				transformations->fromTransformations(object->getTransformations());
				if (translation != nullptr) {
					transformations->getTranslation()->add(translation);
					transformations->update();
				}
				rigidBody->synch(transformations);
				rigidBody->setEnabled(true);
			}
		}
	}
}

extern java::lang::Class* class_(const char16_t* c, int n);

java::lang::Class* Level::class_()
{
    static ::java::lang::Class* c = ::class_(u"tdme.tools.leveleditor.logic.Level", 34);
    return c;
}

void Level::clinit()
{
	super::clinit();
	static bool in_cl_init = false;
	struct clinit_ {
		clinit_() {
			in_cl_init = true;
		compareMutableString = new MutableString();
		transformations = new Transformations();
		}
	};

	if (!in_cl_init) {
		static clinit_ clinit_instance;
	}
}

java::lang::Class* Level::getClass0()
{
	return class_();
}

