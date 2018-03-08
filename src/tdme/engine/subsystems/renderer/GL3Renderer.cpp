#include <tdme/engine/subsystems/renderer/GL3Renderer.h>

#if defined (__APPLE__)
	#include <OpenGL/gl3.h>
#elif defined(__FreeBSD__) or defined(__linux__) or defined(_WIN32)
	#include <GL/glew.h>
#endif

#include <string.h>

#include <array>
#include <vector>
#include <string>

#include <tdme/utils/Buffer.h>
#include <tdme/utils/ByteBuffer.h>
#include <tdme/utils/FloatBuffer.h>
#include <tdme/utils/IntBuffer.h>
#include <tdme/utils/ShortBuffer.h>
#include <tdme/engine/Engine.h>
#include <tdme/engine/fileio/textures/Texture.h>
#include <tdme/math/Matrix4x4.h>
#include <tdme/os/filesystem/FileSystem.h>
#include <tdme/os/filesystem/FileSystemInterface.h>
#include <tdme/utils/Console.h>

using std::array;
using std::vector;
using std::string;
using std::to_string;

using tdme::engine::subsystems::renderer::GL3Renderer;
using tdme::utils::Buffer;
using tdme::utils::ByteBuffer;
using tdme::utils::FloatBuffer;
using tdme::utils::IntBuffer;
using tdme::utils::ShortBuffer;
using tdme::engine::Engine;
using tdme::engine::fileio::textures::Texture;
using tdme::math::Matrix4x4;
using tdme::os::filesystem::FileSystem;
using tdme::os::filesystem::FileSystemInterface;
using tdme::utils::Console;

GL3Renderer::GL3Renderer() 
{
	// setup GL3 consts
	ID_NONE = 0;
	CLEAR_DEPTH_BUFFER_BIT = GL_DEPTH_BUFFER_BIT;
	CLEAR_COLOR_BUFFER_BIT = GL_COLOR_BUFFER_BIT;
	CULLFACE_FRONT = GL_FRONT;
	CULLFACE_BACK = GL_BACK;
	FRONTFACE_CW = GL_CW;
	FRONTFACE_CCW = GL_CCW;
	CLIENTSTATE_TEXTURECOORD_ARRAY = -1;
	CLIENTSTATE_VERTEX_ARRAY = -1;
	CLIENTSTATE_NORMAL_ARRAY = -1;
	CLIENTSTATE_COLOR_ARRAY = -1;
	SHADER_FRAGMENT_SHADER = GL_FRAGMENT_SHADER;
	SHADER_VERTEX_SHADER = GL_VERTEX_SHADER;
	DEPTHFUNCTION_LESSEQUAL = GL_LEQUAL;
	DEPTHFUNCTION_EQUAL = GL_EQUAL;
}

const string GL3Renderer::getGLVersion()
{
	return "gl3";
}

void GL3Renderer::initialize()
{
	glGetError();
	// get default framebuffer
	FRAMEBUFFER_DEFAULT = 0; //getContext()->getDefaultDrawFramebuffer();
	// setup open gl
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glDepthFunc(GL_LEQUAL);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBlendEquation(GL_FUNC_ADD);
	glDisable(GL_BLEND);
	// Note sure here: GLEW requires to have it, whereas I actually do use core profile, maybe something is wrong with FREEGLUT core profile initialization
	#if defined(_WIN32) or defined(__linux__)
		glEnable(GL_POINT_SPRITE);
	#endif
	glEnable(GL_PROGRAM_POINT_SIZE);
	setTextureUnit(0);
	// generate a "engine" VAO as
	//	we do not support VAO's in our engine control flow
	glGenVertexArrays(1, &engineVAO);
	glBindVertexArray(engineVAO);
}

void GL3Renderer::initializeFrame()
{
}

bool GL3Renderer::isBufferObjectsAvailable()
{
	return true;
}

bool GL3Renderer::isDepthTextureAvailable()
{
	return true;
}

bool GL3Renderer::isUsingProgramAttributeLocation()
{
	return false;
}

bool GL3Renderer::isSpecularMappingAvailable()
{
	return true;
}

bool GL3Renderer::isNormalMappingAvailable()
{
	return true;
}

bool GL3Renderer::isDisplacementMappingAvailable()
{
	return false;
}

bool GL3Renderer::isInstancedRenderingAvailable() {
	return true;
}

int32_t GL3Renderer::getTextureUnits()
{
	return -1;
}

int32_t GL3Renderer::loadShader(int32_t type, const string& pathName, const string& fileName)
{
	// create shader
	int32_t handle = glCreateShader(type);
	checkGLError();
	// exit if no handle returned
	if (handle == 0) return 0;

	// shader source
	auto shaderSource = FileSystem::getInstance()->getContentAsString(pathName, fileName);
	string sourceString = (shaderSource);
	char *sourceHeap = new char[sourceString.length() + 1];
	strcpy(sourceHeap, sourceString.c_str());
	// load source
	glShaderSource(handle, 1, &sourceHeap, nullptr);
	// compile
	glCompileShader(handle);
	// check state
	int32_t compileStatus;
	glGetShaderiv(handle, GL_COMPILE_STATUS, &compileStatus);
	if (compileStatus == 0) {
		// get error
		int32_t infoLogLengthBuffer;
		glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &infoLogLengthBuffer);
		char infoLogBuffer[infoLogLengthBuffer];
		glGetShaderInfoLog(handle, infoLogLengthBuffer, &infoLogLengthBuffer, infoLogBuffer);
		auto infoLogString = (string(infoLogBuffer, infoLogLengthBuffer));
		// be verbose
		Console::println(
			string(
				string("GL3Renderer::loadShader") +
				string("[") +
				to_string(handle) +
				string("]") +
				pathName +
				string("/") +
				fileName +
				string(": failed: ") +
				infoLogString
			 )
		 );
		// remove shader
		glDeleteShader(handle);
		return 0;
	}

	return handle;
}

void GL3Renderer::useProgram(int32_t programId)
{
	glUseProgram(programId);
}

int32_t GL3Renderer::createProgram()
{
	auto glProgram = glCreateProgram();
	return glProgram;
}

void GL3Renderer::attachShaderToProgram(int32_t programId, int32_t shaderId)
{
	glAttachShader(programId, shaderId);
}

bool GL3Renderer::linkProgram(int32_t programId)
{
	glLinkProgram(programId);
	// check state
	int32_t linkStatus;
	glGetProgramiv(programId, GL_LINK_STATUS, &linkStatus);
	if (linkStatus == 0) {
		// get error
		int32_t infoLogLength = 0;
		glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &infoLogLength);
		char infoLog[infoLogLength];
		glGetProgramInfoLog(programId, infoLogLength, &infoLogLength, infoLog);
		auto infoLogString = (string(infoLog, infoLogLength));
		// be verbose
		Console::println(
			string(
				"[" +
				to_string(programId) +
				"]: failed: " +
				infoLogString
			 )
		);
		return false;
	}
	return true;
}

int32_t GL3Renderer::getProgramUniformLocation(int32_t programId, const string& name)
{
	auto uniformLocation = glGetUniformLocation(programId, (name).c_str());
	return uniformLocation;
}

void GL3Renderer::setProgramUniformInteger(int32_t uniformId, int32_t value)
{
	glUniform1i(uniformId, value);
}

void GL3Renderer::setProgramUniformFloat(int32_t uniformId, float value)
{
	glUniform1f(uniformId, value);
}

void GL3Renderer::setProgramUniformFloatMatrix4x4(int32_t uniformId, const array<float, 16>& data)
{
	glUniformMatrix4fv(uniformId, 1, false, data.data());
}

void GL3Renderer::setProgramUniformFloatMatrices4x4(int32_t uniformId, int32_t count, FloatBuffer* data)
{
	glUniformMatrix4fv(uniformId, count, false, (float*)data->getBuffer());
}

void GL3Renderer::setProgramUniformFloatVec4(int32_t uniformId, const array<float, 4>& data)
{
	glUniform4fv(uniformId, 1, data.data());
}

void GL3Renderer::setProgramUniformFloatVec3(int32_t uniformId, const array<float, 3>& data)
{
	glUniform3fv(uniformId, 1, data.data());
}

void GL3Renderer::setProgramAttributeLocation(int32_t programId, int32_t location, const string& name)
{
	glBindAttribLocation(programId, location, (name).c_str());
}

void GL3Renderer::setViewPort(int32_t x, int32_t y, int32_t width, int32_t height)
{
	this->viewPortX = x;
	this->viewPortY = x;
	this->viewPortWidth = width;
	this->viewPortHeight = height;
	this->pointSize = width > height ? width / 12.0f : height / 12.0f * 16 / 9;
}

void GL3Renderer::updateViewPort()
{
	glViewport(viewPortX, viewPortY, viewPortWidth, viewPortHeight);
}

void GL3Renderer::setClearColor(float red, float green, float blue, float alpha)
{
	glClearColor(red, green, blue, alpha);
}

void GL3Renderer::enableCulling()
{
	glEnable(GL_CULL_FACE);
}

void GL3Renderer::disableCulling()
{
	glDisable(GL_CULL_FACE);
}

void GL3Renderer::enableBlending()
{
	glEnable(GL_BLEND);
}

void GL3Renderer::disableBlending()
{
	glDisable(GL_BLEND);
}

void GL3Renderer::enableDepthBuffer()
{
	glDepthMask(true);
}

void GL3Renderer::disableDepthBuffer()
{
	glDepthMask(false);
}

void GL3Renderer::setDepthFunction(int32_t depthFunction)
{
	glDepthFunc(depthFunction);
}

void GL3Renderer::setColorMask(bool red, bool green, bool blue, bool alpha)
{
	glColorMask(red, green, blue, alpha);
}

void GL3Renderer::clear(int32_t mask)
{
	glClear(mask);
}

void GL3Renderer::setCullFace(int32_t cullFace)
{
	glCullFace(cullFace);
}

void GL3Renderer::setFrontFace(int32_t frontFace)
{
	glFrontFace(frontFace);
}

int32_t GL3Renderer::createTexture()
{
	// generate open gl texture
	uint32_t textureId;
	glGenTextures(1, &textureId);
	return textureId;
}

int32_t GL3Renderer::createDepthBufferTexture(int32_t width, int32_t height)
{
	uint32_t depthTextureGlId;
	// create depth texture
	glGenTextures(1, &depthTextureGlId);
	glBindTexture(GL_TEXTURE_2D, depthTextureGlId);
	// create depth texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	// depth texture parameter
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	// unbind, return
	glBindTexture(GL_TEXTURE_2D, ID_NONE);
	return depthTextureGlId;
}

int32_t GL3Renderer::createColorBufferTexture(int32_t width, int32_t height)
{
	uint32_t colorBufferTextureGlId;
	// create color texture
	glGenTextures(1, &colorBufferTextureGlId);
	glBindTexture(GL_TEXTURE_2D, colorBufferTextureGlId);
	// create color texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	// color texture parameter
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	// unbind, return
	glBindTexture(GL_TEXTURE_2D, ID_NONE);
	return colorBufferTextureGlId;
}

void GL3Renderer::uploadTexture(Texture* texture)
{
	glTexImage2D(GL_TEXTURE_2D, 0, texture->getDepth() == 32 ? GL_RGBA : GL_RGB, texture->getTextureWidth(), texture->getTextureHeight(), 0, texture->getDepth() == 32 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, texture->getTextureData()->getBuffer());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
}

void GL3Renderer::resizeDepthBufferTexture(int32_t textureId, int32_t width, int32_t height)
{
	glBindTexture(GL_TEXTURE_2D, textureId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void GL3Renderer::resizeColorBufferTexture(int32_t textureId, int32_t width, int32_t height)
{
	glBindTexture(GL_TEXTURE_2D, textureId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void GL3Renderer::bindTexture(int32_t textureId)
{
	glBindTexture(GL_TEXTURE_2D, textureId);
	onBindTexture(textureId);
}

void GL3Renderer::disposeTexture(int32_t textureId)
{
	glDeleteTextures(1, (const uint32_t*)&textureId);
}

int32_t GL3Renderer::createFramebufferObject(int32_t depthBufferTextureGlId, int32_t colorBufferTextureGlId)
{
	uint32_t frameBufferGlId;
	// create a frame buffer object
	glGenFramebuffers(1, &frameBufferGlId);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBufferGlId);
	// attach the depth buffer texture to FBO
	if (depthBufferTextureGlId != ID_NONE) {
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthBufferTextureGlId, 0);
	}
	// attach the depth buffer texture to FBO
	if (colorBufferTextureGlId != ID_NONE) {
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, colorBufferTextureGlId, 0);
		glDrawBuffer(GL_COLOR_ATTACHMENT0);
		glReadBuffer(GL_COLOR_ATTACHMENT0);
	} else {
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
	}
	// check FBO status
	int32_t fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (fboStatus != GL_FRAMEBUFFER_COMPLETE) {
		Console::println(string("GL_FRAMEBUFFER_COMPLETE_EXT failed, CANNOT use FBO: "+ to_string(fboStatus)));
	}
	// switch back to window-system-provided framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return frameBufferGlId;
}

void GL3Renderer::bindFrameBuffer(int32_t frameBufferId)
{
	glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId);
}

void GL3Renderer::disposeFrameBufferObject(int32_t frameBufferId)
{
	glDeleteFramebuffers(1, (uint32_t*)&frameBufferId);
}

vector<int32_t> GL3Renderer::createBufferObjects(int32_t buffers)
{
	vector<int32_t> bufferObjectIds;
	bufferObjectIds.resize(buffers);
	glGenBuffers(buffers, (uint32_t*)bufferObjectIds.data());
	return bufferObjectIds;
}

void GL3Renderer::uploadBufferObject(int32_t bufferObjectId, int32_t size, FloatBuffer* data)
{
	glBindBuffer(GL_ARRAY_BUFFER, bufferObjectId);
	glBufferData(GL_ARRAY_BUFFER, size, data->getBuffer(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, ID_NONE);
}

void GL3Renderer::uploadIndicesBufferObject(int32_t bufferObjectId, int32_t size, ShortBuffer* data)
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferObjectId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data->getBuffer(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID_NONE);
}

void GL3Renderer::uploadBufferObject(int32_t bufferObjectId, int32_t size, ShortBuffer* data)
{
	glBindBuffer(GL_ARRAY_BUFFER, bufferObjectId);
	glBufferData(GL_ARRAY_BUFFER, size, data->getBuffer(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, ID_NONE);
}

void GL3Renderer::bindIndicesBufferObject(int32_t bufferObjectId)
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferObjectId);
}

void GL3Renderer::bindTextureCoordinatesBufferObject(int32_t bufferObjectId)
{
	glBindBuffer(GL_ARRAY_BUFFER, bufferObjectId);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, false, 0, 0LL);
}

void GL3Renderer::bindVerticesBufferObject(int32_t bufferObjectId)
{
	glBindBuffer(GL_ARRAY_BUFFER, bufferObjectId);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0LL);
}

void GL3Renderer::bindNormalsBufferObject(int32_t bufferObjectId)
{
	glBindBuffer(GL_ARRAY_BUFFER, bufferObjectId);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, false, 0, 0LL);
}

void GL3Renderer::bindColorsBufferObject(int32_t bufferObjectId)
{
	glBindBuffer(GL_ARRAY_BUFFER, bufferObjectId);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 4, GL_FLOAT, false, 0, 0LL);
}

void GL3Renderer::bindSkinningVerticesJointsBufferObject(int32_t bufferObjectId)
{
	glBindBuffer(GL_ARRAY_BUFFER, bufferObjectId);
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 1, GL_FLOAT, false, 0, 0LL);
}

void GL3Renderer::bindSkinningVerticesVertexJointsIdxBufferObject(int32_t bufferObjectId)
{
	glBindBuffer(GL_ARRAY_BUFFER, bufferObjectId);
	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, 4, GL_FLOAT, false, 0, 0LL);
}

void GL3Renderer::bindSkinningVerticesVertexJointsWeightBufferObject(int32_t bufferObjectId)
{
	glBindBuffer(GL_ARRAY_BUFFER, bufferObjectId);
	glEnableVertexAttribArray(6);
	glVertexAttribPointer(6, 4, GL_FLOAT, false, 0, 0LL);
}

void GL3Renderer::bindTangentsBufferObject(int32_t bufferObjectId)
{
	glBindBuffer(GL_ARRAY_BUFFER, bufferObjectId);
	glEnableVertexAttribArray(7);
	glVertexAttribPointer(7, 3, GL_FLOAT, false, 0, 0LL);
}

void GL3Renderer::bindBitangentsBufferObject(int32_t bufferObjectId)
{
	glBindBuffer(GL_ARRAY_BUFFER, bufferObjectId);
	glEnableVertexAttribArray(8);
	glVertexAttribPointer(8, 3, GL_FLOAT, false, 0, 0LL);
}

void GL3Renderer::drawIndexedTrianglesFromBufferObjects(int32_t triangles, int32_t trianglesOffset)
{
	#define BUFFER_OFFSET(i) ((void*)(i))
	glDrawElements(GL_TRIANGLES, triangles * 3, GL_UNSIGNED_SHORT, BUFFER_OFFSET(static_cast< int64_t >(trianglesOffset) * 3LL * 2LL));
}

void GL3Renderer::drawTrianglesFromBufferObjects(int32_t triangles, int32_t trianglesOffset)
{
	glDrawArrays(GL_TRIANGLES, trianglesOffset * 3, triangles * 3);
}

void GL3Renderer::drawPointsFromBufferObjects(int32_t points, int32_t pointsOffset)
{
	glDrawArrays(GL_POINTS, pointsOffset, points);
}

void GL3Renderer::unbindBufferObjects()
{
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(3);
	glDisableVertexAttribArray(4);
	glDisableVertexAttribArray(5);
	glDisableVertexAttribArray(6);
	glDisableVertexAttribArray(7);
	glDisableVertexAttribArray(8);
	glBindBuffer(GL_ARRAY_BUFFER, ID_NONE);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID_NONE);
}

void GL3Renderer::disposeBufferObjects(vector<int32_t>* bufferObjectIds)
{
	glDeleteBuffers(bufferObjectIds->size(), (const uint32_t*)bufferObjectIds->data());
}

int32_t GL3Renderer::getTextureUnit()
{
	return activeTextureUnit;
}

void GL3Renderer::setTextureUnit(int32_t textureUnit)
{
	this->activeTextureUnit = textureUnit;
	glActiveTexture(GL_TEXTURE0 + textureUnit);
}

void GL3Renderer::enableClientState(int32_t clientState)
{
}

void GL3Renderer::disableClientState(int32_t clientState)
{
}

float GL3Renderer::readPixelDepth(int32_t x, int32_t y)
{
	/*
	pixelDepthBuffer->clear();
	glReadPixels(x, y, 1, 1, GL_DEPTH_COMPONENT, GL::GL_FLOAT, static_cast< Buffer* >(pixelDepthBuffer));
	return pixelDepthBuffer->get();
	*/
	return -1.0f;
}

ByteBuffer* GL3Renderer::readPixels(int32_t x, int32_t y, int32_t width, int32_t height)
{
	/*
	auto pixelBuffer = ByteBuffer::allocateDirect(width * height * Byte::SIZE* 4);
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glReadPixels(x, y, width, height, GL_RGBA, GL_UNSIGNED_BYTE, static_cast< Buffer* >(pixelBuffer));
	return pixelBuffer;
	*/
	return nullptr;
}

void GL3Renderer::initGuiMode()
{
	setTextureUnit(0);
	glBindTexture(GL_TEXTURE_2D, ID_NONE);
	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glGetError();
}

void GL3Renderer::doneGuiMode()
{
	glGetError();
	glBindTexture(GL_TEXTURE_2D, ID_NONE);
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
}

void GL3Renderer::checkGLError()
{
	auto error = glGetError();
	if (error != GL_NO_ERROR) {
		Console::println(string("OpenGL Error: (" + to_string(error) + ") @:"));
	}
}
