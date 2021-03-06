// based on http://fabiensanglard.net/shadowmapping/index.php, modified by me

#version 100

precision highp float;
precision highp sampler2D;

#define SHADOWMAP_LOOKUPS	2

uniform sampler2D textureUnit;
uniform float texturePixelWidth;
uniform float texturePixelHeight;

uniform sampler2D diffuseTextureUnit;
uniform int diffuseTextureAvailable;
uniform int diffuseTextureMaskedTransparency;
uniform float diffuseTextureMaskedTransparencyThreshold;

uniform vec3 lightDirection;

// passed from vertex shader
varying vec2 vsFragTextureUV;
varying vec4 vsShadowCoord;
varying float vsShadowIntensity;

void main() {
	// retrieve diffuse texture color value
	if (diffuseTextureAvailable == 1) {
		// fetch from texture
		vec4 diffuseTextureColor = texture2D(diffuseTextureUnit, vsFragTextureUV);
		// check if to handle diffuse texture masked transparency
		if (diffuseTextureMaskedTransparency == 1) {
			// discard if beeing transparent
			if (diffuseTextureColor.a < diffuseTextureMaskedTransparencyThreshold) discard;
		}
	}

	// do not process samples out of frustum, or out of shadow map
	if (vsShadowCoord.w == 0.0 ||
		vsShadowCoord.x < 0.0 || vsShadowCoord.x > 1.0 ||
		vsShadowCoord.y < 0.0 || vsShadowCoord.y > 1.0) {
		// return color to be blended with framebuffer
		gl_FragColor = vec4(0.0, 0.0, 0.0, 0.0);
	} else {
		// depth bias
		float depthBias = 0.0;

		// determine visibility
		float visibility = 0.0;
		for (int y = 0; y < SHADOWMAP_LOOKUPS; y++)
		for (int x = 0; x < SHADOWMAP_LOOKUPS; x++) {
			visibility+= texture2D(
				textureUnit,
				vsShadowCoord.xy +
					vec2(
						(float(-SHADOWMAP_LOOKUPS) / 2.0 + 0.5 + float(x)) * texturePixelWidth,
						(float(-SHADOWMAP_LOOKUPS) / 2.0 + 0.5 + float(y)) * texturePixelHeight
					)
			).x < vsShadowCoord.z + depthBias?0.50:0.0;
		}
		visibility = visibility / (float(SHADOWMAP_LOOKUPS) * float(SHADOWMAP_LOOKUPS));

		// return color to be blended with framebuffer
		gl_FragColor = vec4(0.0, 0.0, 0.0, visibility * vsShadowIntensity * 0.5);
	}
}
