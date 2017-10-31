
#pragma once

#include <array>

#include <tdme/tdme.h>
#include <tdme/engine/subsystems/renderer/fwd-tdme.h>

using std::array;

/** 
 * Bean holding material data
 */
struct tdme::engine::subsystems::renderer::GLRenderer_Material
{
	array<float, 4> ambient {{ 0.2f, 0.2f, 0.2f, 1.0f }};
	array<float, 4> diffuse {{ 0.8f, 0.8f, 0.8f, 1.0f }};
	array<float, 4> specular {{ 0.0f, 0.0f, 0.0f, 1.0f }};
	array<float, 4> emission {{ 0.0f, 0.0f, 0.0f, 1.0f }};
	float shininess { 0.0f };
};
