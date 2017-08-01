// Generated from /tdme/src/tdme/engine/Engine.java

#pragma once

#include <fwd-tdme.h>
#include <tdme/engine/fwd-tdme.h>
#include <tdme/engine/subsystems/renderer/GL2Renderer.h>

using tdme::engine::subsystems::renderer::GL2Renderer;
using tdme::engine::Engine;

class tdme::engine::EngineGL2Renderer
	: public GL2Renderer
{

public:
	void onUpdateProjectionMatrix() override;
	void onUpdateCameraMatrix() override;
	void onUpdateModelViewMatrix() override;
	void onBindTexture(int32_t textureId) override;
	void onUpdateTextureMatrix() override;
	void onUpdateEffect() override;
	void onUpdateLight(int32_t lightId) override;
	void onUpdateMaterial() override;

	/**
	 * Public constructor
	 * @param engine
	 */
	EngineGL2Renderer(Engine* engine);
private:
	Engine* engine;
};