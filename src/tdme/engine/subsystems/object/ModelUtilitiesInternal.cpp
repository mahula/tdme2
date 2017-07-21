// Generated from /tdme/src/tdme/engine/subsystems/object/ModelUtilitiesInternal.java
#include <tdme/engine/subsystems/object/ModelUtilitiesInternal.h>

#include <map>
#include <string>

#include <java/lang/Integer.h>
#include <java/lang/Object.h>
#include <java/lang/String.h>
#include <java/util/Iterator.h>
#include <tdme/engine/Timing.h>
#include <tdme/engine/model/AnimationSetup.h>
#include <tdme/engine/model/Face.h>
#include <tdme/engine/model/FacesEntity.h>
#include <tdme/engine/model/Group.h>
#include <tdme/engine/model/Material.h>
#include <tdme/engine/model/Model.h>
#include <tdme/engine/primitives/BoundingBox.h>
#include <tdme/engine/subsystems/object/AnimationState.h>
#include <tdme/engine/subsystems/object/ModelUtilitiesInternal_ModelStatistics.h>
#include <tdme/engine/subsystems/object/Object3DGroup.h>
#include <tdme/engine/subsystems/object/Object3DGroupMesh.h>
#include <tdme/engine/subsystems/object/Object3DModelInternal.h>
#include <tdme/math/Matrix4x4.h>
#include <tdme/math/Vector3.h>
#include <Array.h>
#include <ObjectArray.h>
#include <SubArray.h>

using std::map;
using std::wstring;

using tdme::engine::subsystems::object::ModelUtilitiesInternal;
using java::lang::Integer;
using java::lang::Object;
using java::lang::String;
using java::util::Iterator;
using tdme::engine::Timing;
using tdme::engine::model::AnimationSetup;
using tdme::engine::model::Face;
using tdme::engine::model::FacesEntity;
using tdme::engine::model::Group;
using tdme::engine::model::Material;
using tdme::engine::model::Model;
using tdme::engine::primitives::BoundingBox;
using tdme::engine::subsystems::object::AnimationState;
using tdme::engine::subsystems::object::ModelUtilitiesInternal_ModelStatistics;
using tdme::engine::subsystems::object::Object3DGroup;
using tdme::engine::subsystems::object::Object3DGroupMesh;
using tdme::engine::subsystems::object::Object3DModelInternal;
using tdme::math::Matrix4x4;
using tdme::math::Vector3;

template<typename ComponentType, typename... Bases> struct SubArray;
namespace tdme {
namespace engine {
namespace model {
typedef ::SubArray< ::tdme::engine::model::Face, ::java::lang::ObjectArray > FaceArray;
typedef ::SubArray< ::tdme::engine::model::FacesEntity, ::java::lang::ObjectArray > FacesEntityArray;
}  // namespace model

namespace subsystems {
namespace object {
typedef ::SubArray< ::tdme::engine::subsystems::object::Object3DGroup, ::java::lang::ObjectArray > Object3DGroupArray;
}  // namespace object
}  // namespace subsystems
}  // namespace engine

namespace math {
typedef ::SubArray< ::tdme::math::Vector3, ::java::lang::ObjectArray > Vector3Array;
}  // namespace math
}  // namespace tdme

template<typename T, typename U>
static T java_cast(U* u)
{
    if (!u) return static_cast<T>(nullptr);
    auto t = dynamic_cast<T>(u);
    return t;
}

ModelUtilitiesInternal::ModelUtilitiesInternal(const ::default_init_tag&)
	: super(*static_cast< ::default_init_tag* >(0))
{
	clinit();
}

ModelUtilitiesInternal::ModelUtilitiesInternal()
	: ModelUtilitiesInternal(*static_cast< ::default_init_tag* >(0))
{
	ctor();
}

BoundingBox* ModelUtilitiesInternal::createBoundingBox(Model* model)
{
	clinit();
	return ModelUtilitiesInternal::createBoundingBox(new Object3DModelInternal(model));
}

BoundingBox* ModelUtilitiesInternal::createBoundingBox(Object3DModelInternal* object3DModelInternal)
{
	clinit();
	auto model = object3DModelInternal->getModel();
	auto defaultAnimation = model->getAnimationSetup(Model::ANIMATIONSETUP_DEFAULT);
	float minX = 0.0f, minY = 0.0f, minZ = 0.0f;
	float maxX = 0.0f, maxY = 0.0f, maxZ = 0.0f;
	auto firstVertex = true;
	auto animationState = new AnimationState();
	animationState->setup = defaultAnimation;
	animationState->lastAtTime = Timing::UNDEFINED;
	animationState->currentAtTime = 0LL;
	animationState->time = 0.0f;
	animationState->finished = false;
	for (auto t = 0.0f; t <= (defaultAnimation != nullptr ? static_cast< float >(defaultAnimation->getFrames()) : 0.0f) / model->getFPS(); t += 1.0f / model->getFPS()) {
		object3DModelInternal->computeTransformationsMatrices(model->getSubGroups(), object3DModelInternal->getModel()->getImportTransformationsMatrix()->clone()->multiply(object3DModelInternal->getTransformationsMatrix()), animationState, 0);
		Object3DGroup::computeTransformations(object3DModelInternal->object3dGroups, object3DModelInternal->transformationsMatrices);
		for (auto object3DGroup : *object3DModelInternal->object3dGroups) {
			for (auto vertex : *object3DGroup->mesh->vertices) {
				auto vertexXYZ = vertex->getArray();
				if (firstVertex == true) {
					minX = (*vertexXYZ)[0];
					minY = (*vertexXYZ)[1];
					minZ = (*vertexXYZ)[2];
					maxX = (*vertexXYZ)[0];
					maxY = (*vertexXYZ)[1];
					maxZ = (*vertexXYZ)[2];
					firstVertex = false;
				} else {
					if ((*vertexXYZ)[0] < minX)
						minX = (*vertexXYZ)[0];

					if ((*vertexXYZ)[1] < minY)
						minY = (*vertexXYZ)[1];

					if ((*vertexXYZ)[2] < minZ)
						minZ = (*vertexXYZ)[2];

					if ((*vertexXYZ)[0] > maxX)
						maxX = (*vertexXYZ)[0];

					if ((*vertexXYZ)[1] > maxY)
						maxY = (*vertexXYZ)[1];

					if ((*vertexXYZ)[2] > maxZ)
						maxZ = (*vertexXYZ)[2];

				}
			}
		}
		animationState->currentAtTime = static_cast< int64_t >((t * 1000.0f));
		animationState->lastAtTime = static_cast< int64_t >((t * 1000.0f));
	}
	if (firstVertex == true)
		return nullptr;

	return new BoundingBox(new Vector3(minX, minY, minZ), new Vector3(maxX, maxY, maxZ));
}

void ModelUtilitiesInternal::invertNormals(Model* model)
{
	clinit();
	invertNormals(model->getSubGroups());
}

void ModelUtilitiesInternal::invertNormals(map<wstring, Group*>* groups)
{
	clinit();
	for (auto it: *groups) {
		Group* group = it.second;
		for (auto normal : *group->getNormals()) {
			normal->scale(-1.0f);
		}
		invertNormals(group->getSubGroups());
	}
}

ModelUtilitiesInternal_ModelStatistics* ModelUtilitiesInternal::computeModelStatistics(Model* model)
{
	clinit();
	return ModelUtilitiesInternal::computeModelStatistics(new Object3DModelInternal(model));
}

ModelUtilitiesInternal_ModelStatistics* ModelUtilitiesInternal::computeModelStatistics(Object3DModelInternal* object3DModelInternal)
{
	clinit();
	map<wstring, int32_t> materialCountById;
	auto opaqueFaceCount = 0;
	auto transparentFaceCount = 0;
	for (auto object3DGroup : *object3DModelInternal->object3dGroups) {
		auto facesEntities = object3DGroup->group->getFacesEntities();
		auto facesEntityIdxCount = facesEntities->length;
		for (auto faceEntityIdx = 0; faceEntityIdx < facesEntityIdxCount; faceEntityIdx++) {
			auto facesEntity = (*facesEntities)[faceEntityIdx];
			auto faces = facesEntity->getFaces()->length;
			auto material = facesEntity->getMaterial();
			auto transparentFacesEntity = false;
			if (material != nullptr) {
				if (material->hasTransparency() == true)
					transparentFacesEntity = true;

			}
			auto materialId = material == nullptr ? u"tdme.material.none"_j : material->getId();
			materialCountById[materialId->getCPPWString()]++;
			if (transparentFacesEntity == true) {
				transparentFaceCount += faces;
				continue;
			}
			opaqueFaceCount += faces;
		}
	}
	auto materialCount = materialCountById.size();
	return new ModelUtilitiesInternal_ModelStatistics(opaqueFaceCount, transparentFaceCount, materialCount);
}

bool ModelUtilitiesInternal::equals(Model* model1, Model* model2)
{
	clinit();
	return ModelUtilitiesInternal::equals(new Object3DModelInternal(model1), new Object3DModelInternal(model2));
}

bool ModelUtilitiesInternal::equals(Object3DModelInternal* object3DModel1Internal, Object3DModelInternal* object3DModel2Internal)
{
	clinit();
	if (object3DModel1Internal->object3dGroups->length != object3DModel2Internal->object3dGroups->length)
		return false;

	for (auto i = 0; i < object3DModel1Internal->object3dGroups->length; i++) {
		auto object3DGroupModel1 = (*object3DModel1Internal->object3dGroups)[i];
		auto object3DGroupModel2 = (*object3DModel2Internal->object3dGroups)[i];
		auto facesEntitiesModel1 = object3DGroupModel1->group->getFacesEntities();
		auto facesEntitiesModel2 = object3DGroupModel2->group->getFacesEntities();
		if (object3DGroupModel1->group->getTransformationsMatrix()->equals(object3DGroupModel2->group->getTransformationsMatrix()) == false)
			return false;

		if (facesEntitiesModel1->length != facesEntitiesModel2->length)
			return false;

		for (auto j = 0; j < facesEntitiesModel1->length; j++) {
			auto facesEntityModel1 = (*facesEntitiesModel1)[j];
			auto facesEntityModel2 = (*facesEntitiesModel2)[j];
			if (facesEntityModel1->getMaterial() == nullptr && facesEntityModel2->getMaterial() != nullptr)
				return false;

			if (facesEntityModel1->getMaterial() != nullptr && facesEntityModel2->getMaterial() == nullptr)
				return false;

			if (facesEntityModel1->getMaterial() != nullptr && facesEntityModel2->getMaterial() != nullptr && facesEntityModel1->getMaterial()->getId()->equals(facesEntityModel2->getMaterial()->getId()) == false) {
				return false;
			}
			auto facesModel1 = facesEntityModel1->getFaces();
			auto facesModel2 = facesEntityModel2->getFaces();
			if (facesModel1->length != facesModel2->length)
				return false;

			for (auto k = 0; k < facesModel1->length; k++) {
				auto vertexIndicesModel1 = (*facesModel1)[k]->getVertexIndices();
				auto vertexIndicesModel2 = (*facesModel2)[k]->getVertexIndices();
				if ((*vertexIndicesModel1)[0] != (*vertexIndicesModel2)[0] || (*vertexIndicesModel1)[1] != (*vertexIndicesModel2)[1] || (*vertexIndicesModel1)[2] != (*vertexIndicesModel2)[2]) {
					return false;
				}
			}
		}
	}
	return true;
}

extern java::lang::Class* class_(const char16_t* c, int n);

java::lang::Class* ModelUtilitiesInternal::class_()
{
    static ::java::lang::Class* c = ::class_(u"tdme.engine.subsystems.object.ModelUtilitiesInternal", 52);
    return c;
}

bool ModelUtilitiesInternal::equals(Object* obj)
{
	return super::equals(obj);
}

java::lang::Class* ModelUtilitiesInternal::getClass0()
{
	return class_();
}

