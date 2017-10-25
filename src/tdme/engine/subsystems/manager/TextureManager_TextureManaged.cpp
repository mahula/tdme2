#include <tdme/engine/subsystems/manager/TextureManager_TextureManaged.h>

#include <string>

#include <tdme/engine/subsystems/manager/TextureManager.h>

using std::string;

using tdme::engine::subsystems::manager::TextureManager_TextureManaged;
using tdme::engine::subsystems::manager::TextureManager;

TextureManager_TextureManaged::TextureManager_TextureManaged(const string& id, int32_t glId)
{
	this->id = id;
	this->glId = glId;
	this->referenceCounter = 0;
}

const string& TextureManager_TextureManaged::getId()
{
	return id;
}

int32_t TextureManager_TextureManaged::getGlId()
{
	return glId;
}

int32_t TextureManager_TextureManaged::getReferenceCounter()
{
	return referenceCounter;
}

bool TextureManager_TextureManaged::decrementReferenceCounter()
{
	referenceCounter--;
	return referenceCounter == 0;
}

void TextureManager_TextureManaged::incrementReferenceCounter()
{
	referenceCounter++;
}
