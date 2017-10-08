
#pragma once

#include <tdme.h>
#include <tdme/engine/fwd-tdme.h>

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
	 * @param gl
	 */
	void initialize();

	/** 
	 * Resize the frame buffer
	 * @param gl
	 */
	void reshape(int32_t width, int32_t height);

	/** 
	 * Disposes this frame buffer
	 * @param gl
	 */
	void dispose();

	/** 
	 * Enables this frame buffer to be rendered
	 * @param gl
	 */
	void enableFrameBuffer();

	/** 
	 * Switches back to non offscreen main frame buffer to be rendered
	 * @param gl
	 */
	static void disableFrameBuffer();

	/** 
	 * Bind depth texture
	 * @param gl
	 */
	void bindDepthBufferTexture();

	/** 
	 * @return color buffer texture id
	 */
	int32_t getColorBufferTextureId();

	/**
	 * Public constructor
	 * @param engine
	 * @param width
	 * @param height
	 * @param buffers (see FrameBuffer::FRAMEBUFFER_*)
	 */
	FrameBuffer(int32_t width, int32_t height, int32_t buffers);
};
