#include <tdme/engine/subsystems/shadowmapping/ShadowMappingShaderPreDefaultImplementation.h>

#include <tdme/engine/subsystems/renderer/GLRenderer.h>
#include <tdme/os/filesystem/FileSystem.h>
#include <tdme/os/filesystem/FileSystemInterface.h>

using tdme::engine::subsystems::shadowmapping::ShadowMappingShaderPreDefaultImplementation;
using tdme::engine::subsystems::renderer::GLRenderer;
using tdme::os::filesystem::FileSystem;
using tdme::os::filesystem::FileSystemInterface;

bool ShadowMappingShaderPreDefaultImplementation::isSupported(GLRenderer* renderer) {
	return true;
}

ShadowMappingShaderPreDefaultImplementation::ShadowMappingShaderPreDefaultImplementation(GLRenderer* renderer): ShadowMappingShaderPreBaseImplementation(renderer)
{
}

ShadowMappingShaderPreDefaultImplementation::~ShadowMappingShaderPreDefaultImplementation() {
}

void ShadowMappingShaderPreDefaultImplementation::initialize()
{
	auto rendererVersion = renderer->getGLVersion();

	// load shadow mapping shaders
	//	pre render
	vertexShaderGlId = renderer->loadShader(
		renderer->SHADER_VERTEX_SHADER,
		"shader/" + rendererVersion + "/shadowmapping",
		"pre_vertexshader.c",
		"",
		FileSystem::getInstance()->getContentAsString(
			"shader/" + rendererVersion + "/shadowmapping",
			"pre_computevertex.inc.c"
		)
	);
	if (vertexShaderGlId == 0) return;
	fragmentShaderGlId = renderer->loadShader(
		renderer->SHADER_FRAGMENT_SHADER,
		"shader/" + rendererVersion + "/shadowmapping",
		"pre_fragmentshader.c"
	);
	if (fragmentShaderGlId == 0) return;

	// create shadow mapping render program
	//	pre
	programGlId = renderer->createProgram();
	renderer->attachShaderToProgram(programGlId, vertexShaderGlId);
	renderer->attachShaderToProgram(programGlId, fragmentShaderGlId);

	//
	ShadowMappingShaderPreBaseImplementation::initialize();
}
