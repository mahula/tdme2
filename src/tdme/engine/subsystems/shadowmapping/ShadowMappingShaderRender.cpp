#include <tdme/engine/subsystems/shadowmapping/ShadowMappingShaderRender.h>

#include <tdme/engine/Engine.h>
#include <tdme/engine/Timing.h>
#include <tdme/engine/subsystems/renderer/Renderer.h>
#include <tdme/engine/subsystems/shadowmapping/ShadowMappingShaderRenderImplementation.h>
#include <tdme/engine/subsystems/shadowmapping/ShadowMappingShaderRenderDefaultImplementation.h>
#include <tdme/engine/subsystems/shadowmapping/ShadowMappingShaderRenderFoliageImplementation.h>
#include <tdme/math/Matrix4x4.h>

using tdme::engine::Engine;
using tdme::engine::Timing;
using tdme::engine::subsystems::shadowmapping::ShadowMappingShaderRender;
using tdme::engine::subsystems::shadowmapping::ShadowMappingShaderRenderImplementation;
using tdme::engine::subsystems::shadowmapping::ShadowMappingShaderRenderDefaultImplementation;
using tdme::engine::subsystems::shadowmapping::ShadowMappingShaderRenderFoliageImplementation;
using tdme::engine::subsystems::renderer::Renderer;
using tdme::math::Matrix4x4;

ShadowMappingShaderRender::ShadowMappingShaderRender(Renderer* renderer) 
{
	if (ShadowMappingShaderRenderDefaultImplementation::isSupported(renderer) == true) shader["default"] = new ShadowMappingShaderRenderDefaultImplementation(renderer);
	if (ShadowMappingShaderRenderFoliageImplementation::isSupported(renderer) == true) shader["foliage"] = new ShadowMappingShaderRenderFoliageImplementation(renderer);
}

ShadowMappingShaderRender::~ShadowMappingShaderRender()
{
	for (auto shaderIt: shader) {
		delete shaderIt.second;
	}
}

bool ShadowMappingShaderRender::isInitialized()
{
	bool initialized = true;
	for (auto shaderIt: shader) {
		initialized&= shaderIt.second->isInitialized();
	}
	return initialized;
}

void ShadowMappingShaderRender::initialize()
{
	for (auto shaderIt: shader) {
		shaderIt.second->initialize();
	}
}

void ShadowMappingShaderRender::useProgram(Engine* engine)
{
	running = true;
	this->engine = engine;
}

void ShadowMappingShaderRender::unUseProgram()
{
	running = false;
	if (implementation != nullptr) {
		implementation->unUseProgram();;
	}
	implementation = nullptr;
	engine = nullptr;
}

void ShadowMappingShaderRender::setProgramMVMatrix(const Matrix4x4& mvMatrix)
{
	if (implementation == nullptr) return;
	implementation->setProgramMVMatrix(mvMatrix);
}

void ShadowMappingShaderRender::setProgramMVPMatrix(const Matrix4x4& mvpMatrix)
{
	if (implementation == nullptr) return;
	implementation->setProgramMVPMatrix(mvpMatrix);
}

void ShadowMappingShaderRender::setProgramNormalMatrix(const Matrix4x4& normalMatrix)
{
	if (implementation == nullptr) return;
	implementation->setProgramNormalMatrix(normalMatrix);
}

void ShadowMappingShaderRender::updateTextureMatrix(Renderer* renderer) {
	if (implementation == nullptr) return;
	implementation->updateTextureMatrix(renderer);
}

void ShadowMappingShaderRender::updateMaterial(Renderer* renderer)
{
	if (implementation == nullptr) return;
	implementation->updateMaterial(renderer);
}

void ShadowMappingShaderRender::updateLight(Renderer* renderer, int32_t lightId) {
	if (implementation == nullptr) return;
	implementation->updateLight(renderer, lightId);
}

void ShadowMappingShaderRender::bindTexture(Renderer* renderer, int32_t textureId)
{
	if (implementation == nullptr) return;
	implementation->bindTexture(renderer, textureId);
}

void ShadowMappingShaderRender::setProgramDepthBiasMVPMatrix(const Matrix4x4& depthBiasMVPMatrix)
{
	this->depthBiasMVPMatrix = depthBiasMVPMatrix;
	if (implementation == nullptr) return;
	implementation->setProgramDepthBiasMVPMatrix(this->depthBiasMVPMatrix);
}

void ShadowMappingShaderRender::setRenderLightId(int32_t lightId) {
	this->lightId = lightId;
}

void ShadowMappingShaderRender::setShader(const string& id) {
	auto currentImplementation = implementation;

	auto shaderIt = shader.find(id);
	if (shaderIt == shader.end()) {
		shaderIt = shader.find("default");
	}
	implementation = shaderIt->second;

	if (currentImplementation != implementation) {
		if (currentImplementation != nullptr) currentImplementation->unUseProgram();
		implementation->useProgram(engine);
	}

	implementation->setProgramDepthBiasMVPMatrix(depthBiasMVPMatrix);
	implementation->setRenderLightId(lightId);
}
