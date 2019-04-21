#include <tdme/engine/LODObject3D.h>

#include <string>

#include <tdme/engine/Engine.h>
#include <tdme/engine/Object3D.h>
#include <tdme/engine/Transformations.h>
#include <tdme/engine/Partition.h>
#include <tdme/utils/Console.h>

using std::string;

using tdme::engine::LODObject3D;
using tdme::engine::Engine;
using tdme::engine::Object3D;
using tdme::engine::Partition;
using tdme::engine::Transformations;
using tdme::utils::Console;

LODObject3D::LODObject3D(
	const string& id,
	Model* modelLOD1,
	LODLevelType levelTypeLOD2,
	float modelLOD2MinDistance,
	Model* modelLOD2,
	LODLevelType levelTypeLOD3,
	float modelLOD3MinDistance,
	Model* modelLOD3,
	float planeRotationYLOD2,
	float planeRotationYLOD3
):
	id(id),
	modelLOD1(modelLOD1),
	levelTypeLOD2(levelTypeLOD2),
	modelLOD2MinDistance(modelLOD2MinDistance),
	modelLOD2(modelLOD2),
	levelTypeLOD3(levelTypeLOD3),
	modelLOD3MinDistance(modelLOD3MinDistance),
	modelLOD3(modelLOD3),
	planeRotationYLOD2(planeRotationYLOD2),
	planeRotationYLOD3(planeRotationYLOD3)
{
	this->enabled = true;
	this->pickable = false;
	this->dynamicShadowing = false;
	this->effectColorMul.set(1.0f, 1.0f, 1.0f, 1.0f);
	this->effectColorAdd.set(0.0f, 0.0f, 0.0f, 0.0f);
	this->effectColorMulLOD2.set(1.0f, 1.0f, 1.0f, 1.0f);
	this->effectColorAddLOD2.set(0.0f, 0.0f, 0.0f, 0.0f);
	this->effectColorMulLOD3.set(1.0f, 1.0f, 1.0f, 1.0f);
	this->effectColorAddLOD3.set(0.0f, 0.0f, 0.0f, 0.0f);
	transformationsRotationYIndex = -1;

	if (modelLOD1 != nullptr) objectLOD1 = new Object3D(id + ".lod1", modelLOD1);
	if (modelLOD2 != nullptr) objectLOD2 = new Object3D(id + ".lod2", modelLOD2);
	if (modelLOD3 != nullptr) objectLOD3 = new Object3D(id + ".lod3", modelLOD3);

	if (objectLOD1 != nullptr) objectLOD1->setShader(shaderId);
	if (objectLOD2 != nullptr) objectLOD2->setShader(shaderId);
	if (objectLOD3 != nullptr) objectLOD3->setShader(shaderId);
	if (objectLOD1 != nullptr) objectLOD1->setDistanceShader(distanceShaderId);
	if (objectLOD2 != nullptr) objectLOD2->setDistanceShader(distanceShaderId);
	if (objectLOD3 != nullptr) objectLOD3->setDistanceShader(distanceShaderId);
	if (objectLOD1 != nullptr) objectLOD1->setDistanceShaderDistance(distanceShaderDistance);
	if (objectLOD2 != nullptr) objectLOD2->setDistanceShaderDistance(distanceShaderDistance);
	if (objectLOD3 != nullptr) objectLOD3->setDistanceShaderDistance(distanceShaderDistance);

	levelLOD = 1;
}

void LODObject3D::setEngine(Engine* engine)
{
	this->engine = engine;
}

void LODObject3D::setRenderer(GLRenderer* renderer)
{
}

void LODObject3D::fromTransformations(const Transformations& transformations)
{
	Transformations::fromTransformations(transformations);
	// determine y axis if required
	if (levelTypeLOD2 == LODLEVELTYPE_PLANE || levelTypeLOD3 == LODLEVELTYPE_PLANE) {
		transformationsRotationYIndex = -1;
		for (auto i = 0; i < this->getRotationCount(); i++) {
			if (this->getRotationAxis(i).equals(Rotation::Y_AXIS) == true) {
				transformationsRotationYIndex = i;
				break;
			}
		}
	}
	// delegate to LOD objects
	if (objectLOD1 != nullptr) objectLOD1->fromTransformations(*this);
	if (objectLOD2 != nullptr) objectLOD2->fromTransformations(*this);
	if (objectLOD3 != nullptr) objectLOD3->fromTransformations(*this);
	// update entity
	if (frustumCulling == true && engine != nullptr && enabled == true) engine->partition->updateEntity(this);
	// reset current LOD object
	objectLOD = nullptr;
}

void LODObject3D::update()
{
	Transformations::update();
	// determine y axis if required
	if (levelTypeLOD2 == LODLEVELTYPE_PLANE || levelTypeLOD3 == LODLEVELTYPE_PLANE) {
		transformationsRotationYIndex = -1;
		for (auto i = 0; i < this->getRotationCount(); i++) {
			if (this->getRotationAxis(i).equals(Rotation::Y_AXIS) == true) {
				transformationsRotationYIndex = i;
				break;
			}
		}
	}
	// delegate to LOD objects
	if (objectLOD1 != nullptr) objectLOD1->fromTransformations(*this);
	if (objectLOD2 != nullptr) objectLOD2->fromTransformations(*this);
	if (objectLOD3 != nullptr) objectLOD3->fromTransformations(*this);
	// update entity
	if (frustumCulling == true && engine != nullptr && enabled == true) engine->partition->updateEntity(this);
	// reset current LOD object
	objectLOD = nullptr;
}

void LODObject3D::setEnabled(bool enabled)
{
	// return if enable state has not changed
	if (this->enabled == enabled) return;
	// frustum culling enabled?
	if (frustumCulling == true) {
		// yeo, add or remove from partition
		if (enabled == true) {
			if (engine != nullptr) engine->partition->addEntity(this);
		} else {
			if (engine != nullptr) engine->partition->removeEntity(this);
		}
	}
	// reset current LOD object
	objectLOD = nullptr;
}

bool LODObject3D::isFrustumCulling() {
	return frustumCulling;
}

void LODObject3D::setFrustumCulling(bool frustumCulling) {
	// check if enabled and engine attached
	if (enabled == true && engine != nullptr) {
		// had frustum culling
		if (this->frustumCulling == true) {
			// yep, remove if set to false now
			if (frustumCulling == false) engine->partition->removeEntity(this);
		} else {
			// yep, add if set to true now
			if (frustumCulling == true) engine->partition->addEntity(this);
		}
	}
	this->frustumCulling = frustumCulling;
	// delegate change to engine
	if (engine != nullptr) engine->updateEntity(this);
}

void LODObject3D::dispose()
{
	// delegate to LOD objects
	if (objectLOD1 != nullptr) objectLOD1->dispose();
	if (objectLOD2 != nullptr) objectLOD2->dispose();
	if (objectLOD3 != nullptr) objectLOD3->dispose();
}

void LODObject3D::initialize()
{
	// delegate to LOD objects
	if (objectLOD1 != nullptr) objectLOD1->initialize();
	if (objectLOD2 != nullptr) objectLOD2->initialize();
	if (objectLOD3 != nullptr) objectLOD3->initialize();
}
