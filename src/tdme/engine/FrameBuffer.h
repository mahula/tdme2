#pragma once

#include <string>

#include <tdme/tdme.h>
#include <tdme/engine/fwd-tdme.h>
#include <tdme/engine/subsystems/postprocessing/fwd-tdme.h>

using std::string;

using tdme::engine::subsystems::postprocessing::PostProcessingShader;

/** 
 * Frame buffer class
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::engine::FrameBuffer final
{

public:
	static constexpr int32_t FRAMEBUFFER_DEPTHBUFFER { 1 };
	static constexpr int32_t FRAMEBUFFER_COLORBUFFER { 2 };

private:
	int32_t width {  };
	int32_t height {  };
	int32_t frameBufferId {  };
	int32_t depthBufferTextureId {  };
	int32_t colorBufferTextureId {  };
	int32_t buffers {  };
public:

	/** 
	 * @return width
	 */
	int32_t getWidth();

	/** 
	 * @return height
	 */
	int32_t getHeight();

	/** 
	 * Init the frame buffer
	 * currently only depth buffer rendering is enabled, can be extended to color buffer easily
	 */
	void initialize();

	/** 
	 * Resize the frame buffer
	 * @param width width
	 * @param height height
	 */
	void reshape(int32_t width, int32_t height);

	/** 
	 * Disposes this frame buffer
	 */
	void dispose();

	/** 
	 * Enables this frame buffer to be rendered
	 */
	void enableFrameBuffer();

	/** 
	 * Switches back to non offscreen main frame buffer to be rendered
	 */
	static void disableFrameBuffer();

	/** 
	 * Bind depth texture
	 */
	void bindDepthBufferTexture();

	/** 
	 * @return color buffer texture id
	 */
	int32_t getColorBufferTextureId();

	/**
	 * @return depth buffer texture id
	 */
	int32_t getDepthBufferTextureId();

	/**
	 * Render to screen
	 */
	void renderToScreen();

	/**
	 * Do post processing into this frame buffer
	 * @param source source frame buffer
	 * @param post processing shader id
	 */
	void doPostProcessing(FrameBuffer* source, const string& shaderId);

	/**
	 * Public constructor
	 * @param width width
	 * @param height height
	 * @param buffers buffers (see FrameBuffer::FRAMEBUFFER_*)
	 */
	FrameBuffer(int32_t width, int32_t height, int32_t buffers);
};
