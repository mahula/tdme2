#include <tdme/engine/subsystems/shadowmapping/ShadowMappingShaderPreBaseImplementation.h>

#include <tdme/engine/Engine.h>
#include <tdme/engine/Timing.h>
#include <tdme/engine/subsystems/lighting/LightingShader.h>
#include <tdme/engine/subsystems/lighting/LightingShaderConstants.h>
#include <tdme/engine/subsystems/renderer/GLRenderer.h>
#include <tdme/math/Matrix4x4.h>
#include <tdme/os/filesystem/FileSystem.h>
#include <tdme/os/filesystem/FileSystemInterface.h>
#include <tdme/utils/Console.h>

using tdme::engine::Engine;
using tdme::engine::Timing;
using tdme::engine::subsystems::shadowmapping::ShadowMappingShaderPreBaseImplementation;
using tdme::engine::subsystems::lighting::LightingShader;
using tdme::engine::subsystems::lighting::LightingShaderConstants;
using tdme::engine::subsystems::renderer::GLRenderer;
using tdme::math::Matrix4x4;
using tdme::os::filesystem::FileSystem;
using tdme::os::filesystem::FileSystemInterface;
using tdme::utils::Console;

ShadowMappingShaderPreBaseImplementation::ShadowMappingShaderPreBaseImplementation(GLRenderer* renderer)
{
	this->renderer = renderer;
	initialized = false;
}

ShadowMappingShaderPreBaseImplementation::~ShadowMappingShaderPreBaseImplementation() {
}

bool ShadowMappingShaderPreBaseImplementation::isInitialized()
{
	return initialized;
}

void ShadowMappingShaderPreBaseImplementation::initialize()
{
	// map inputs to attributes
	if (renderer->isUsingProgramAttributeLocation() == true) {
		renderer->setProgramAttributeLocation(programId, 0, "inVertex");
		renderer->setProgramAttributeLocation(programId, 2, "inTextureUV");
	}
	// link
	if (renderer->linkProgram(programId) == false) return;

	// uniforms
	if (renderer->isInstancedRenderingAvailable() == true) {
		//	uniforms
		uniformProjectionMatrix = renderer->getProgramUniformLocation(programId, "projectionMatrix");
		if (uniformProjectionMatrix == -1) return;
		uniformCameraMatrix = renderer->getProgramUniformLocation(programId, "cameraMatrix");
		if (uniformCameraMatrix == -1) return;
	} else {
		//	uniforms
		uniformMVPMatrix = renderer->getProgramUniformLocation(programId, "mvpMatrix");
		if (uniformMVPMatrix == -1) return;
	}
	uniformTextureMatrix = renderer->getProgramUniformLocation(programId, "textureMatrix");
	if (uniformTextureMatrix == -1) return;
	uniformDiffuseTextureUnit = renderer->getProgramUniformLocation(programId, "diffuseTextureUnit");
	if (uniformDiffuseTextureUnit == -1) return;
	uniformDiffuseTextureAvailable = renderer->getProgramUniformLocation(programId, "diffuseTextureAvailable");
	if (uniformDiffuseTextureAvailable == -1) return;
	uniformDiffuseTextureMaskedTransparency = renderer->getProgramUniformLocation(programId, "diffuseTextureMaskedTransparency");
	if (uniformDiffuseTextureMaskedTransparency == -1) return;
	uniformDiffuseTextureMaskedTransparencyThreshold = renderer->getProgramUniformLocation(programId, "diffuseTextureMaskedTransparencyThreshold");
	if (uniformDiffuseTextureMaskedTransparencyThreshold == -1) return;

	//
	uniformFrame = renderer->getProgramUniformLocation(programId, "frame");

	//
	initialized = true;
}

void ShadowMappingShaderPreBaseImplementation::useProgram(Engine* engine)
{
	renderer->useProgram(programId);
	renderer->setProgramUniformInteger(uniformDiffuseTextureUnit, LightingShaderConstants::TEXTUREUNIT_DIFFUSE);
	if (uniformFrame != -1) renderer->setProgramUniformInteger(uniformFrame, engine->getTiming()->getFrame());
}

void ShadowMappingShaderPreBaseImplementation::unUseProgram()
{
}

void ShadowMappingShaderPreBaseImplementation::updateMatrices(const Matrix4x4& mvpMatrix)
{
	if (renderer->isInstancedRenderingAvailable() == true) {
		renderer->setProgramUniformFloatMatrix4x4(uniformProjectionMatrix, renderer->getProjectionMatrix().getArray());
		renderer->setProgramUniformFloatMatrix4x4(uniformCameraMatrix, renderer->getCameraMatrix().getArray());
	} else {
		renderer->setProgramUniformFloatMatrix4x4(uniformMVPMatrix, mvpMatrix.getArray());
	}
}

void ShadowMappingShaderPreBaseImplementation::updateTextureMatrix(GLRenderer* renderer) {
	renderer->setProgramUniformFloatMatrix3x3(uniformTextureMatrix, renderer->getTextureMatrix().getArray());
}

void ShadowMappingShaderPreBaseImplementation::updateMaterial(GLRenderer* renderer)
{
	renderer->setProgramUniformInteger(uniformDiffuseTextureMaskedTransparency, renderer->material.diffuseTextureMaskedTransparency);
	renderer->setProgramUniformFloat(uniformDiffuseTextureMaskedTransparencyThreshold, renderer->material.diffuseTextureMaskedTransparencyThreshold);
}

void ShadowMappingShaderPreBaseImplementation::bindTexture(GLRenderer* renderer, int32_t textureId)
{
	switch (renderer->getTextureUnit()) {
		case LightingShaderConstants::TEXTUREUNIT_DIFFUSE:
			renderer->setProgramUniformInteger(uniformDiffuseTextureAvailable, textureId == 0 ? 0 : 1);
			break;
	}
}
