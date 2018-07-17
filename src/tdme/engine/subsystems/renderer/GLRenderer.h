#pragma once

#include <array>
#include <vector>
#include <string>

#include <tdme/tdme.h>
#include <tdme/utils/fwd-tdme.h>
#include <tdme/engine/fileio/textures/fwd-tdme.h>
#include <tdme/engine/subsystems/renderer/fwd-tdme.h>
#include <tdme/engine/subsystems/renderer/GLRenderer_Light.h>
#include <tdme/engine/subsystems/renderer/GLRenderer_Material.h>
#include <tdme/math/fwd-tdme.h>
#include <tdme/math/Matrix2D3x3.h>
#include <tdme/math/Matrix4x4.h>

using std::array;
using std::vector;
using std::string;

using tdme::utils::ByteBuffer;
using tdme::utils::FloatBuffer;
using tdme::utils::IntBuffer;
using tdme::utils::ShortBuffer;
using tdme::engine::fileio::textures::Texture;
using tdme::engine::subsystems::renderer::GLRenderer_Light;
using tdme::engine::subsystems::renderer::GLRenderer_Material;
using tdme::math::Matrix2D3x3;
using tdme::math::Matrix4x4;

/** 
 * OpenGL renderer interface
 * @author Andreas Drewke
 * @ersion $Id$
 */
class tdme::engine::subsystems::renderer::GLRenderer
{
public:
	int32_t ID_NONE {  };
	int32_t CLEAR_DEPTH_BUFFER_BIT {  };
	int32_t CLEAR_COLOR_BUFFER_BIT {  };
	int32_t CULLFACE_FRONT {  };
	int32_t CULLFACE_BACK {  };
	int32_t TEXTUREUNITS_MAX {  };
	int32_t SHADER_FRAGMENT_SHADER {  };
	int32_t SHADER_VERTEX_SHADER {  };
	int32_t SHADER_GEOMETRY_SHADER {  };
	int32_t SHADER_COMPUTE_SHADER {  };
	int32_t DEPTHFUNCTION_ALWAYS {  };
	int32_t DEPTHFUNCTION_EQUAL {  };
	int32_t DEPTHFUNCTION_LESSEQUAL {  };
	int32_t DEPTHFUNCTION_GREATEREQUAL {  };
	int32_t FRAMEBUFFER_DEFAULT {  };
	int32_t FRONTFACE_CW {  };
	int32_t FRONTFACE_CCW {  };
	array<float, 4> effectColorMul {{ 1.0f, 1.0f, 1.0f, 1.0f }};
	array<float, 4> effectColorAdd {{ 0.0f, 0.0f, 0.0f, 0.0f }};
	GLRenderer_Material material;
	array<GLRenderer_Light, 8> lights;
	bool applyFoliageAnimation {  };
	int frame { 0 };

protected:
	int32_t viewPortX {  };
	int32_t viewPortY {  };
	int32_t viewPortWidth {  };
	int32_t viewPortHeight {  };
	int32_t activeTextureUnit {  };
	Matrix4x4 projectionMatrix {  };
	Matrix4x4 cameraMatrix {  };
	Matrix4x4 modelViewMatrix {  };
	Matrix4x4 viewportMatrix {  };
	Matrix2D3x3 textureMatrix {  };

public:
	float pointSize {  };

	/** 
	 * Initialize renderer
	 */
	virtual void initialize() = 0;

	/** 
	 * Pre Frame Initialization
	 */
	virtual void initializeFrame();

	/** 
	 * @return renderer version e.g. gl2, gl3 or gles2
	 */
	virtual const string getGLVersion() = 0;

	/** 
	 * Checks if buffer objects is available
	 * @return buffer objects availability
	 */
	virtual bool isBufferObjectsAvailable() = 0;

	/** 
	 * Checks if depth texture is available
	 * @return depth texture is available
	 */
	virtual bool isDepthTextureAvailable() = 0;

	/** 
	 * @return requires program attribute location
	 */
	virtual bool isUsingProgramAttributeLocation() = 0;

	/** 
	 * @return if specular mapping is supported
	 */
	virtual bool isSpecularMappingAvailable() = 0;

	/** 
	 * @return if normal mapping is supported
	 */
	virtual bool isNormalMappingAvailable() = 0;

	/** 
	 * @return if displacement mapping is supported
	 */
	virtual bool isDisplacementMappingAvailable() = 0;

	/** 
	 * Checks if instanced rendering is available
	 * @return instance rendering availability
	 */
	virtual bool isInstancedRenderingAvailable() = 0;

	/**
	 * @return Returns if renderer is using short indices, otherwise it uses int indices
	 */
	virtual bool isUsingShortIndices() = 0;

	/**
	 * @return If geometry shader is available
	 */
	virtual bool isGeometryShaderAvailable() = 0;

	/**
	 * @return number of texture units
	 */
	virtual int32_t getTextureUnits() = 0;

	/** 
	 * Loads a shader into open gl stack
	 * @param gl
	 * @param type
	 * @param pathName
	 * @param fileName
	 * @return shader handle
	 */
	virtual int32_t loadShader(int32_t type, const string& pathName, const string& fileName) = 0;

	/** 
	 * Use shader program
	 * @param programId
	 */
	virtual void useProgram(int32_t programId) = 0;

	/** 
	 * Creates a shader program
	 * @return int
	 */
	virtual int32_t createProgram() = 0;

	/** 
	 * Attaches a shader to a program
	 * @param program id
	 * @param shader id
	 */
	virtual void attachShaderToProgram(int32_t programId, int32_t shaderId) = 0;

	/** 
	 * Links attached shaders to a program
	 * @param program id
	 * @return success
	 */
	virtual bool linkProgram(int32_t programId) = 0;

	/** 
	 * Returns location of given uniform variable
	 * @param program id
	 * @param uniform name
	 * @return
	 */
	virtual int32_t getProgramUniformLocation(int32_t programId, const string& name) = 0;

	/** 
	 * Set up a integer uniform value
	 * @param uniform id
	 * @param value
	 */
	virtual void setProgramUniformInteger(int32_t uniformId, int32_t value) = 0;

	/** 
	 * Set up a float uniform value
	 * @param uniform id
	 * @param value
	 */
	virtual void setProgramUniformFloat(int32_t uniformId, float value) = 0;

	/** 
	 * Set up a float matrix 3x3 uniform value
	 * @param uniform id
	 * @param value
	 */
	virtual void setProgramUniformFloatMatrix3x3(int32_t uniformId, const array<float, 9>& value) = 0;

	/**
	 * Set up a float matrix 4x4 uniform value
	 * @param uniform id
	 * @param value
	 */
	virtual void setProgramUniformFloatMatrix4x4(int32_t uniformId, const array<float, 16>& value) = 0;

	/** 
	 * Set up a float matrices 4x4 uniform values
	 * @param uniform id
	 * @param count
	 * @param data
	 */
	virtual void setProgramUniformFloatMatrices4x4(int32_t uniformId, int32_t count, FloatBuffer* data) = 0;

	/** 
	 * Set up a float vec4 uniform value
	 * @param uniform id
	 * @param data
	 */
	virtual void setProgramUniformFloatVec4(int32_t uniformId, const array<float, 4>& data) = 0;

	/** 
	 * Set up a float vec3 uniform value
	 * @param uniform id
	 * @param data
	 */
	virtual void setProgramUniformFloatVec3(int32_t uniformId, const array<float, 3>& data) = 0;

	/** 
	 * Bind attribute to a input location
	 * @param program id
	 * @param location
	 * @param attribute name
	 */
	virtual void setProgramAttributeLocation(int32_t programId, int32_t location, const string& name) = 0;

	/** 
	 * Set up viewport parameter
	 * @param x
	 * @param y
	 * @param width
	 * @param height
	 */
	virtual void setViewPort(int32_t x, int32_t y, int32_t width, int32_t height) = 0;

	/** 
	 * Update viewport
	 */
	virtual void updateViewPort() = 0;

	/** 
	 * @return projection matrix
	 */
	virtual Matrix4x4& getProjectionMatrix();

	/** 
	 * Update projection matrix event
	 */
	virtual void onUpdateProjectionMatrix() = 0;

	/** 
	 * @return camera matrix
	 */
	virtual Matrix4x4& getCameraMatrix();

	/** 
	 * Update camera matrix event
	 */
	virtual void onUpdateCameraMatrix() = 0;

	/** 
	 * @return model view matrix
	 */
	virtual Matrix4x4& getModelViewMatrix();

	/** 
	 * Update model view matrix event
	 */
	virtual void onUpdateModelViewMatrix() = 0;

	/** 
	 * @return view port matrix
	 */
	virtual Matrix4x4& getViewportMatrix();

	/** 
	 * @return texture matrix
	 */
	virtual Matrix2D3x3& getTextureMatrix();

	/**
	 * Update texture matrix for active texture unit event
	 */
	virtual void onUpdateTextureMatrix() = 0;

	/** 
	 * Set up clear color
	 * @param red
	 * @param green
	 * @param blue
	 * @param alpha
	 */
	virtual void setClearColor(float red, float green, float blue, float alpha) = 0;

	/** 
	 * Enable culling
	 */
	virtual void enableCulling() = 0;

	/** 
	 * Disable culling
	 */
	virtual void disableCulling() = 0;

	/** 
	 * Enables blending
	 */
	virtual void enableBlending() = 0;

	/** 
	 * Disables blending
	 */
	virtual void disableBlending() = 0;

	/** 
	 * Enable depth buffer
	 */
	virtual void enableDepthBuffer() = 0;

	/** 
	 * Disable depth buffer
	 */
	virtual void disableDepthBuffer() = 0;

	/** 
	 * Set up depth function
	 * @param depth function
	 */
	virtual void setDepthFunction(int32_t depthFunction) = 0;

	/** 
	 * Set up GL rendering colormask
	 * @param red
	 * @param green
	 * @param blue
	 * @param alpha
	 */
	virtual void setColorMask(bool red, bool green, bool blue, bool alpha) = 0;

	/** 
	 * Clear render buffer with given mask
	 * @param mask
	 */
	virtual void clear(int32_t mask) = 0;

	/**
	 * Sets up which face will be culled
	 * @param cull face
	 */
	virtual void setCullFace(int32_t cullFace) = 0;

	/** 
	 * Set up clock wise or counter clock wise faces as front face 
	 * @param frontFace
	 */
	virtual void setFrontFace(int32_t frontFace) = 0;

	/** 
	 * Creates a texture
	 * @return texture id
	 */
	virtual int32_t createTexture() = 0;

	/** 
	 * Creates a depth buffer texture
	 * @param width
	 * @param height
	 * @return depth texture id
	 */
	virtual int32_t createDepthBufferTexture(int32_t width, int32_t height) = 0;

	/** 
	 * Creates a color buffer texture
	 * @param width
	 * @param height
	 * @return color buffer texture id
	 */
	virtual int32_t createColorBufferTexture(int32_t width, int32_t height) = 0;

	/** 
	 * Uploads texture data to current bound texture
	 * @param texture
	 */
	virtual void uploadTexture(Texture* texture) = 0;

	/** 
	 * Resizes a depth texture
	 * @param texture id
	 * @param width
	 * @param height
	 */
	virtual void resizeDepthBufferTexture(int32_t textureId, int32_t width, int32_t height) = 0;

	/** 
	 * Resize color buffer texture
	 * @param texture id
	 * @param width
	 * @param height
	 */
	virtual void resizeColorBufferTexture(int32_t textureId, int32_t width, int32_t height) = 0;

	/** 
	 * Binds a texture with given id or unbinds when using ID_NONE
	 * @param textureId
	 */
	virtual void bindTexture(int32_t textureId) = 0;

	/** 
	 * On bind texture event
	 * @param textureId
	 */
	virtual void onBindTexture(int32_t textureId) = 0;

	/** 
	 * Dispose a texture
	 * @param texture id
	 */
	virtual void disposeTexture(int32_t textureId) = 0;

	/** 
	 * Creates a frame buffer object with depth texture attached
	 * @param colorBufferTextureGlId TODO
	 * @param depth texture gl id
	 * @return frame buffer object id
	 */
	virtual int32_t createFramebufferObject(int32_t depthBufferTextureGlId, int32_t colorBufferTextureGlId) = 0;

	/** 
	 * Enables a framebuffer to be rendered
	 * @param frameBufferId
	 */
	virtual void bindFrameBuffer(int32_t frameBufferId) = 0;

	/** 
	 * Disposes a frame buffer object
	 * @param frame buffer id
	 */
	virtual void disposeFrameBufferObject(int32_t frameBufferId) = 0;

	/** 
	 * Generate buffer objects for vertex data and such
	 * @param buffers
	 * @return ids
	 */
	virtual vector<int32_t> createBufferObjects(int32_t buffers) = 0;

	/** 
	 * Uploads buffer data to buffer object
	 * @param buffer object id
	 * @param size
	 * @param data
	 */
	virtual void uploadBufferObject(int32_t bufferObjectId, int32_t size, FloatBuffer* data) = 0;

	/** 
	 * Uploads buffer data to buffer object
	 * @param buffer object id
	 * @param size
	 * @param data
	 */
	virtual void uploadIndicesBufferObject(int32_t bufferObjectId, int32_t size, ShortBuffer* data) = 0;

	/** 
	 * Uploads buffer data to buffer object
	 * @param buffer object id
	 * @param size
	 * @param data
	 */
	virtual void uploadIndicesBufferObject(int32_t bufferObjectId, int32_t size, IntBuffer* data) = 0;

	/** 
	 * Bind indices buffer object
	 * @param buffer object id
	 */
	virtual void bindIndicesBufferObject(int32_t bufferObjectId) = 0;

	/** 
	 * Bind texture coordinates buffer object
	 * @param buffer object id
	 */
	virtual void bindTextureCoordinatesBufferObject(int32_t bufferObjectId) = 0;

	/** 
	 * Bind vertices buffer object
	 * @param buffer object id
	 */
	virtual void bindVerticesBufferObject(int32_t bufferObjectId) = 0;

	/** 
	 * Bind normals buffer object
	 * @param buffer object id
	 */
	virtual void bindNormalsBufferObject(int32_t bufferObjectId) = 0;

	/** 
	 * Bind colors buffer object
	 * @param buffer object id
	 */
	virtual void bindColorsBufferObject(int32_t bufferObjectId) = 0;

	/** 
	 * Bind tangents buffer object
	 * @param buffer object id
	 */
	virtual void bindTangentsBufferObject(int32_t bufferObjectId) = 0;

	/** 
	 * Bind bitangents buffer object
	 * @param buffer object id
	 */
	virtual void bindBitangentsBufferObject(int32_t bufferObjectId) = 0;

	/** 
	 * Bind model matrices buffer object
	 * @param buffer object id
	 */
	virtual void bindModelMatricesBufferObject(int32_t bufferObjectId) = 0;

	/**
	 * Bind effect color muls buffer object
	 * @param buffer object id
	 */
	virtual void bindEffectColorMulsBufferObject(int32_t bufferObjectId) = 0;

	/**
	 * Bind effect color adds buffer object
	 * @param buffer object id
	 */
	virtual void bindEffectColorAddsBufferObject(int32_t bufferObjectId) = 0;

	/**
	 * Draw instanced indexed triangles from buffer objects
	 * @param triangles
	 * @param triangles offset
	 * @param instances
	 */
	virtual void drawInstancedIndexedTrianglesFromBufferObjects(int32_t triangles, int32_t trianglesOffset, int32_t instances) = 0;

	/**
	 * Draw indexed triangles from buffer objects
	 * @param triangles
	 * @param triangles offset
	 */
	virtual void drawIndexedTrianglesFromBufferObjects(int32_t triangles, int32_t trianglesOffset) = 0;

	/** 
	 * Draw instanced triangles from buffer objects
	 * @param triangles
	 * @param triangles offset
	 */
	virtual void drawInstancedTrianglesFromBufferObjects(int32_t triangles, int32_t trianglesOffset, int32_t instances) = 0;

	/** 
	 * Draw triangles from buffer objects
	 * @param triangles
	 * @param triangles offset
	 */
	virtual void drawTrianglesFromBufferObjects(int32_t triangles, int32_t trianglesOffset) = 0;

	/**
	 * Draw points from buffer objects 
	 * @param points
	 * @param points offset
	 */
	virtual void drawPointsFromBufferObjects(int32_t points, int32_t pointsOffset) = 0;

	/** 
	 * Unbind buffer objects
	 */
	virtual void unbindBufferObjects() = 0;

	/** 
	 * Disposes a frame buffer object
	 * @param frame buffer id
	 */
	virtual void disposeBufferObjects(vector<int32_t>* bufferObjectIds) = 0;

	/** 
	 * @return active texture unit
	 */
	virtual int32_t getTextureUnit() = 0;

	/** 
	 * Sets up texture unit
	 * @param texture unit
	 */
	virtual void setTextureUnit(int32_t textureUnit) = 0;

	/** 
	 * Enable light
	 * @param light id
	 */
	virtual void setLightEnabled(int32_t lightId);

	/** 
	 * Disable light
	 * @param light id
	 */
	virtual void setLightDisabled(int32_t lightId);

	/** 
	 * Set light ambient color
	 * @param light id
	 * @param ambient
	 */
	virtual void setLightAmbient(int32_t lightId, const array<float, 4>& ambient);

	/** 
	 * Set light diffuse color
	 * @param light id
	 * @param diffuse
	 */
	virtual void setLightDiffuse(int32_t lightId, const array<float, 4>& diffuse);

	/** 
	 * Set light position
	 * @param light id
	 * @param position
	 */
	virtual void setLightPosition(int32_t lightId, const array<float, 4>& position);

	/** 
	 * Set light spot direction
	 * @param light id
	 * @param spot direction
	 */
	virtual void setLightSpotDirection(int32_t lightId, const array<float, 3>& spotDirection);

	/** 
	 * Set light spot exponent
	 * @param light id
	 * @param spot exponent
	 */
	virtual void setLightSpotExponent(int32_t lightId, float spotExponent);

	/** 
	 * Set light spot cut off
	 * @param light id
	 * @param spot cut off
	 */
	virtual void setLightSpotCutOff(int32_t lightId, float spotCutOff);

	/** 
	 * Set light constant attenuation
	 * @param light id
	 * @param constant attenuation
	 */
	virtual void setLightConstantAttenuation(int32_t lightId, float constantAttenuation);

	/** 
	 * Set light linear attenuation
	 * @param light id
	 * @param linear attenuation
	 */
	virtual void setLightLinearAttenuation(int32_t lightId, float linearAttenuation);

	/** 
	 * Set light quadratic attenuation
	 * @param light id
	 * @param quadratic attenuation
	 */
	virtual void setLightQuadraticAttenuation(int32_t lightId, float QuadraticAttenuation);

	/** 
	 * Update light
	 * @param light id
	 */
	virtual void onUpdateLight(int32_t lightId) = 0;

	/** 
	 * Set up effect color multiplication
	 * @param effect color for multiplication
	 */
	virtual void setEffectColorMul(const array<float, 4>& effectColorMul);

	/** 
	 * Set up effect color addition
	 * @param effect color for addition
	 */
	virtual void setEffectColorAdd(const array<float, 4>& effectColorAdd);

	/** 
	 * Update material
	 */
	virtual void onUpdateEffect() = 0;

	/** 
	 * Enable materials
	 */
	virtual void setMaterialEnabled();

	/** 
	 * Disable materials
	 */
	virtual void setMaterialDisabled();

	/** 
	 * Set material ambient color
	 * @param ambient
	 */
	virtual void setMaterialAmbient(const array<float, 4>& ambient);

	/** 
	 * Set material diffuse color
	 * @param diffuse
	 */
	virtual void setMaterialDiffuse(const array<float, 4>& diffuse);

	/** 
	 * Set material specular color
	 * @param specular
	 */
	virtual void setMaterialSpecular(const array<float, 4>& specular);

	/** 
	 * Set material emission color
	 * @param emission
	 */
	virtual void setMaterialEmission(const array<float, 4>& emission);

	/** 
	 * Set material shininess
	 * @param shininess
	 */
	virtual void setMaterialShininess(float shininess);

	/**
	 * Set material diffuse texture masked transparency
	 */
	virtual void setMaterialDiffuseTextureMaskedTransparency(bool diffuseTextureMaskedTransparency);

	/**
	 * Set material diffuse texture masked transparency threshold
	 */
	virtual void setMaterialDiffuseTextureMaskedTransparencyThreshold(float diffuseTextureMaskedTransparencyThreshold);

	/** 
	 * On update material
	 */
	virtual void onUpdateMaterial() = 0;

	/**
	 * Set apply foliage animation
	 * @param apply foliage animation
	 */
	virtual void setApplyFoliageAnimation(bool applyFoliageAnimation);

	/**
	 * On update apply foliage animation
	 */
	virtual void onUpdateApplyFoliageAnimation() = 0;

	/** 
	 * Reads a pixel depth
	 * @param x
	 * @param y
	 * @return depth 0.0f..1.0f
	 */
	virtual float readPixelDepth(int32_t x, int32_t y) = 0;

	/**
	 * Read pixels
	 * @param x
	 * @param y
	 * @param width
	 * @param height
	 * @return byte buffer
	 */
	virtual ByteBuffer* readPixels(int32_t x, int32_t y, int32_t width, int32_t height) = 0;

	/**
	 * Dispatch compute
	 * @param num groups x
	 * @param num groups y
	 * @param num groups z
	 */
	virtual void dispatchCompute(int32_t numGroupsX, int32_t numGroupsY, int32_t numGroupsZ) = 0;

	/**
	 * Memory barrier
	 */
	virtual void memoryBarrier() = 0;

	/**
	 * Upload skinning buffer object
	 * @param buffer object id
	 * @param size
	 * @param data
	 */
	virtual void uploadSkinningBufferObject(int32_t bufferObjectId, int32_t size, FloatBuffer* data) = 0;

	/**
	 * Upload skinning buffer object
	 * @param buffer object id
	 * @param size
	 * @param data
	 */
	virtual void uploadSkinningBufferObject(int32_t bufferObjectId, int32_t size, IntBuffer* data) = 0;

	/**
	 * Bind skinning vertices buffer object
	 * @param buffer object id
	 */
	virtual void bindSkinningVerticesBufferObject(int32_t bufferObjectId) = 0;

	/**
	 * Bind skinning normal buffer object
	 * @param buffer object id
	 */
	virtual void bindSkinningNormalsBufferObject(int32_t bufferObjectId) = 0;

	/**
	 * Bind skinning vertex joints buffer object
	 * @param buffer object id
	 */
	virtual void bindSkinningVertexJointsBufferObject(int32_t bufferObjectId) = 0;

	/**
	 * Bind skinning vertex joint indices buffer object
	 * @param buffer object id
	 */
	virtual void bindSkinningVertexJointIdxsBufferObject(int32_t bufferObjectId) = 0;

	/**
	 * Bind skinning vertex joint weights buffer object
	 * @param buffer object id
	 */
	virtual void bindSkinningVertexJointWeightsBufferObject(int32_t bufferObjectId) = 0;

	/**
	 * Bind skinning vertices result buffer object
	 * @param buffer object id
	 */
	virtual void bindSkinningVerticesResultBufferObject(int32_t bufferObjectId) = 0;

	/**
	 * Bind skinning normals result buffer object
	 * @param buffer object id
	 */
	virtual void bindSkinningNormalsResultBufferObject(int32_t bufferObjectId) = 0;

	/**
	 * Bind skinning matrices result buffer objec
	 * @param buffer object id
	 */
	virtual void bindSkinningMatricesBufferObject(int32_t bufferObjectId) = 0;

	/** 
	 * Set up renderer for GUI rendering
	 */
	virtual void initGuiMode() = 0;

	/** 
	 * Set up renderer for 3d rendering
	 */
	virtual void doneGuiMode() = 0;

	/**
	 * Public constructor
	 */
	GLRenderer();

	/**
	 * Destructor
	 */
	virtual ~GLRenderer();

private:
	/**
	 * Init
	 */
	void init();

	//
	friend class GLRenderer_Material;
	friend class GLRenderer_Light;
};
