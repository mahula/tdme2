#include <tdme/tools/shared/model/LevelEditorEntity.h>

#include <vector>

#include <tdme/engine/model/Model.h>
#include <tdme/math/Vector3.h>
#include <tdme/tools/shared/model/LevelEditorEntity_EntityType.h>
#include <tdme/tools/shared/model/LevelEditorEntityBoundingVolume.h>
#include <tdme/tools/shared/model/LevelEditorEntityParticleSystem.h>

using std::vector;

using tdme::tools::shared::model::LevelEditorEntity;
using tdme::engine::model::Model;
using tdme::math::Vector3;
using tdme::tools::shared::model::LevelEditorEntity_EntityType;
using tdme::tools::shared::model::LevelEditorEntityBoundingVolume;
using tdme::tools::shared::model::LevelEditorEntityParticleSystem;

constexpr int32_t LevelEditorEntity::ID_NONE;

LevelEditorEntity::LevelEditorEntity(int32_t id, LevelEditorEntity_EntityType* entityType, const string& name, const string& description, const string& entityFileName, const string& fileName, const string& thumbnail, Model* model, const Vector3& pivot)
{
	this->id = id;
	this->type = entityType;
	this->name = name;
	this->description = description;
	this->entityFileName = entityFileName;
	this->fileName = fileName;
	this->thumbnail = thumbnail;
	this->model = model;
	this->pivot.set(pivot);
	if (this->type == LevelEditorEntity_EntityType::PARTICLESYSTEM) {
		this->particleSystem = new LevelEditorEntityParticleSystem();
	}
}

LevelEditorEntity::~LevelEditorEntity() {
	if (model != nullptr) delete model;
}

int32_t LevelEditorEntity::getId()
{
	return id;
}

LevelEditorEntity_EntityType* LevelEditorEntity::getType()
{
	return type;
}

const string& LevelEditorEntity::getName()
{
	return name;
}

void LevelEditorEntity::setName(const string& name)
{
	this->name = name;
}

const string& LevelEditorEntity::getDescription()
{
	return description;
}

void LevelEditorEntity::setDescription(const string& description)
{
	this->description = description;
}

const string& LevelEditorEntity::getEntityFileName()
{
	return entityFileName;
}

void LevelEditorEntity::setEntityFileName(const string& entityFileName)
{
	this->entityFileName = entityFileName;
}

const string& LevelEditorEntity::getFileName()
{
	return fileName;
}

const string& LevelEditorEntity::getThumbnail()
{
	return thumbnail;
}

Model* LevelEditorEntity::getModel()
{
	return model;
}

Vector3& LevelEditorEntity::getPivot()
{
	return pivot;
}

int32_t LevelEditorEntity::getBoundingVolumeCount()
{
	return boundingVolumes.size();
}

LevelEditorEntityBoundingVolume* LevelEditorEntity::getBoundingVolumeAt(int32_t idx)
{
	return boundingVolumes.at(idx);
}

bool LevelEditorEntity::addBoundingVolume(int32_t idx, LevelEditorEntityBoundingVolume* levelEditorEntityBoundingVolume)
{
	if (idx < 0)
		return false;

	if (idx > boundingVolumes.size())
		return false;

	if (idx == boundingVolumes.size()) {
		boundingVolumes.push_back(levelEditorEntityBoundingVolume);
	}
	return false;
}

void LevelEditorEntity::setDefaultBoundingVolumes()
{
	for (auto i = boundingVolumes.size(); i < 8; i++) {
		auto bv = new LevelEditorEntityBoundingVolume(i, this);
		addBoundingVolume(i, bv);
	}
}

LevelEditorEntityParticleSystem* LevelEditorEntity::getParticleSystem()
{
	return particleSystem;
}
