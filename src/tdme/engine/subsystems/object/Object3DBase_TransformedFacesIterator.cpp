// Generated from /tdme/src/tdme/engine/subsystems/object/Object3DBase.java
#include <tdme/engine/subsystems/object/Object3DBase_TransformedFacesIterator.h>

#include <Array.h>
#include <tdme/engine/model/Face.h>
#include <tdme/engine/model/FacesEntity.h>
#include <tdme/engine/model/Group.h>
#include <tdme/engine/subsystems/object/Object3DBase.h>
#include <tdme/engine/subsystems/object/Object3DGroup.h>
#include <tdme/engine/subsystems/object/Object3DGroupMesh.h>
#include <tdme/math/Matrix4x4.h>
#include <tdme/math/Vector3.h>

using tdme::engine::subsystems::object::Object3DBase_TransformedFacesIterator;
using tdme::engine::model::Face;
using tdme::engine::model::FacesEntity;
using tdme::engine::model::Group;
using tdme::engine::subsystems::object::Object3DBase;
using tdme::engine::subsystems::object::Object3DGroup;
using tdme::engine::subsystems::object::Object3DGroupMesh;
using tdme::math::Matrix4x4;
using tdme::math::Vector3;

Object3DBase_TransformedFacesIterator::Object3DBase_TransformedFacesIterator(Object3DBase* object3DBase) 
{
	this->object3DBase = object3DBase;
	this->matrix = new Matrix4x4();
	reset();
}

void Object3DBase_TransformedFacesIterator::reset()
{
	faceCount = 0;
	for (auto object3DGroup : object3DBase->object3dGroups) {
		for (auto facesEntity : *object3DGroup->group->getFacesEntities()) {
			faceCount += facesEntity.getFaces()->size();
		}
	}
	faceIdx = 0;
	faceIdxTotal = 0;
	object3DGroupIdx = 0;
	facesEntityIdx = 0;
	auto object3DGroup = object3DBase->object3dGroups[object3DGroupIdx];
	matrix = (object3DGroup->mesh->skinning == true ? matrix->identity() : matrix->set(object3DGroup->groupTransformationsMatrix))->multiply(object3DBase->transformationsMatrix);
}

Object3DBase_TransformedFacesIterator* Object3DBase_TransformedFacesIterator::iterator() {
	reset();
	return this;
}

bool Object3DBase_TransformedFacesIterator::hasNext()
{
	return faceIdxTotal < faceCount;
}

array<Vector3, 3>* Object3DBase_TransformedFacesIterator::next()
{
	auto object3DGroup = object3DBase->object3dGroups[object3DGroupIdx];
	auto facesEntities = object3DGroup->group->getFacesEntities();
	auto& facesEntity = (*facesEntities)[facesEntityIdx];
	auto faces = facesEntity.getFaces();
	auto& face = (*faces)[faceIdx];
	auto faceVertexIndices = face.getVertexIndices();
	auto groupVerticesTransformed = object3DGroup->mesh->vertices;
	matrix->multiply(&(*groupVerticesTransformed)[(*faceVertexIndices)[0]], &vertices[0]);
	matrix->multiply(&(*groupVerticesTransformed)[(*faceVertexIndices)[1]], &vertices[1]);
	matrix->multiply(&(*groupVerticesTransformed)[(*faceVertexIndices)[2]], &vertices[2]);
	faceIdxTotal++;
	faceIdx++;
	if (faceIdxTotal < faceCount) {
		if (faceIdx == faces->size()) {
			faceIdx = 0;
			facesEntityIdx++;
			if (facesEntityIdx == facesEntities->size()) {
				facesEntityIdx = 0;
				object3DGroupIdx++;
				object3DGroup = object3DBase->object3dGroups[object3DGroupIdx];
				matrix = (object3DGroup->mesh->skinning == true ? matrix->identity() : matrix->set(object3DGroup->groupTransformationsMatrix))->multiply(object3DBase->transformationsMatrix);
			}
		}
	}
	return &vertices;
}

