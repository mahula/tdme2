#include <tdme/tools/shared/model/LevelEditorEntityParticleSystem_PointParticleSystem.h>

#include <string>

#include <tdme/engine/fileio/textures/Texture.h>
#include <tdme/engine/fileio/textures/TextureLoader.h>
#include <tdme/tools/shared/tools/Tools.h>

using std::string;

using tdme::engine::fileio::textures::Texture;
using tdme::engine::fileio::textures::TextureLoader;
using tdme::tools::shared::tools::Tools;

using tdme::tools::shared::model::LevelEditorEntityParticleSystem_PointParticleSystem;

LevelEditorEntityParticleSystem_PointParticleSystem::LevelEditorEntityParticleSystem_PointParticleSystem() 
{
	maxPoints = 4000;
	pointSize = 10.0f;
	textureFileName = "";
	texture = nullptr;
	autoEmit = true;
}

void LevelEditorEntityParticleSystem_PointParticleSystem::setTextureFileName(const string& textureFileName) {
	if (textureFileName.empty() == true) {
		if (texture != nullptr) delete texture;
		texture = nullptr;
		this->textureFileName.clear();
		return;
	}
	this->textureFileName = textureFileName;
	texture = TextureLoader::loadTexture(
		Tools::getPath(textureFileName),
		Tools::getFileName(textureFileName)
	);

}

LevelEditorEntityParticleSystem_PointParticleSystem::~LevelEditorEntityParticleSystem_PointParticleSystem() {
	if (texture != nullptr) delete texture;
}
