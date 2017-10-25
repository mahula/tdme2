#include <tdme/engine/model/Model.h>

#include <map>
#include <string>

#include <tdme/engine/ModelUtilities.h>
#include <tdme/engine/Object3DModel.h>
#include <tdme/engine/model/Animation.h>
#include <tdme/engine/model/AnimationSetup.h>
#include <tdme/engine/model/Group.h>
#include <tdme/engine/model/Material.h>
#include <tdme/engine/model/Model_UpVector.h>
#include <tdme/engine/model/RotationOrder.h>
#include <tdme/engine/primitives/BoundingBox.h>
#include <tdme/engine/subsystems/object/Object3DModelInternal.h>
#include <tdme/math/Matrix4x4.h>

using std::map;
using std::string;

using tdme::engine::model::Model;
using tdme::engine::ModelUtilities;
using tdme::engine::Object3DModel;
using tdme::engine::model::Animation;
using tdme::engine::model::AnimationSetup;
using tdme::engine::model::Group;
using tdme::engine::model::Material;
using tdme::engine::model::Model_UpVector;
using tdme::engine::model::RotationOrder;
using tdme::engine::primitives::BoundingBox;
using tdme::engine::subsystems::object::Object3DModelInternal;
using tdme::math::Matrix4x4;

string Model::ANIMATIONSETUP_DEFAULT = "tdme.default";

constexpr float Model::FPS_DEFAULT;

Model::Model(const string& id, const string& name, Model_UpVector* upVector, RotationOrder* rotationOrder, BoundingBox* boundingBox)
{
	this->id = id;
	this->name = name;
	this->upVector = upVector;
	this->rotationOrder = rotationOrder;
	hasSkinning_ = false;
	fps = FPS_DEFAULT;
	importTransformationsMatrix.identity();
	this->boundingBox = boundingBox;
}

Model::~Model() {
	for (auto it = materials.begin(); it != materials.end(); ++it) {
		delete it->second;
	}
	for (auto it = animationSetups.begin(); it != animationSetups.end(); ++it) {
		delete it->second;
	}
	deleteSubGroups(&subGroups);
	if (boundingBox != nullptr) delete boundingBox;
}

void Model::deleteSubGroups(map<string, Group*>* subGroups) {
	for (auto it = subGroups->begin(); it != subGroups->end(); ++it) {
		deleteSubGroups(it->second->getSubGroups());
		delete it->second;
	}
}

const string& Model::getId()
{
	return id;
}

const string& Model::getName()
{
	return name;
}

Model_UpVector* Model::getUpVector()
{
	return upVector;
}

RotationOrder* Model::getRotationOrder()
{
	return rotationOrder;
}

map<string, Material*>* Model::getMaterials()
{
	return &materials;
}

map<string, Group*>* Model::getGroups()
{
	return &groups;
}

Group* Model::getGroupById(const string& id)
{
	auto groupIt = groups.find(id);
	if (groupIt != groups.end()) {
		return groupIt->second;
	}
	return nullptr;

}

map<string, Group*>* Model::getSubGroups()
{
	return &subGroups;
}

Group* Model::getSubGroupById(const string& id)
{
	auto groupIt = subGroups.find(id);
	if (groupIt != subGroups.end()) {
		return groupIt->second;
	}
	return nullptr;
}

bool Model::hasSkinning()
{
	return hasSkinning_;
}

void Model::setHasSkinning(bool hasSkinning)
{
	this->hasSkinning_ = hasSkinning;
}

float Model::getFPS()
{
	return fps;
}

void Model::setFPS(float fps)
{
	this->fps = fps;
}

AnimationSetup* Model::addAnimationSetup(const string& id, int32_t startFrame, int32_t endFrame, bool loop)
{
	auto animationSetup = new AnimationSetup(this, id, startFrame, endFrame, loop, "");
	animationSetups[id] = animationSetup;
	return animationSetup;
}

AnimationSetup* Model::addOverlayAnimationSetup(const string& id, const string& overlayFromGroupId, int32_t startFrame, int32_t endFrame, bool loop)
{
	auto animationSetup = new AnimationSetup(this, id, startFrame, endFrame, loop, overlayFromGroupId);
	animationSetups[id] = animationSetup;
	return animationSetup;
}

AnimationSetup* Model::getAnimationSetup(const string& id)
{
	auto animationSetupIt = animationSetups.find(id);
	if (animationSetupIt != animationSetups.end()) {
		return animationSetupIt->second;
	}
	return nullptr;
}

map<string, AnimationSetup*>* Model::getAnimationSetups()
{
	return &animationSetups;
}

bool Model::hasAnimations()
{
	return animationSetups.size() > 0;
}

Matrix4x4& Model::getImportTransformationsMatrix()
{
	return importTransformationsMatrix;
}

BoundingBox* Model::getBoundingBox()
{
	if (boundingBox == nullptr) {
		boundingBox = ModelUtilities::createBoundingBox(static_cast< Object3DModelInternal* >(new Object3DModel(this)));
	}
	return boundingBox;
}

bool Model::computeTransformationsMatrix(map<string, Group*>* groups, Matrix4x4& parentTransformationsMatrix, int32_t frame, const string& groupId, Matrix4x4& transformationsMatrix)
{
	for (auto it: *groups) {
		Group* group = it.second;
		auto animation = group->getAnimation();
		if (animation != nullptr) {
			auto animationMatrices = animation->getTransformationsMatrices();
			transformationsMatrix.set((*animationMatrices)[frame % animationMatrices->size()]);
			transformationsMatrix.multiply(group->getTransformationsMatrix());
		} else {
			transformationsMatrix.set(group->getTransformationsMatrix());
		}
		transformationsMatrix.multiply(parentTransformationsMatrix);
		if (group->getId() == groupId) return true;

		auto subGroups = group->getSubGroups();
		if (subGroups->size() > 0) {
			Matrix4x4 parentTransformationsMatrix = transformationsMatrix;
			auto haveTransformationsMatrix = computeTransformationsMatrix(subGroups, parentTransformationsMatrix, frame, groupId, transformationsMatrix);
			if (haveTransformationsMatrix == true) return true;
		}
	}

	return false;
}
