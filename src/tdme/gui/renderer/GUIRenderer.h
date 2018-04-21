#pragma once

#include <array>
#include <vector>

#include <tdme/tdme.h>
#include <tdme/utils/fwd-tdme.h>
#include <tdme/utils/FloatBuffer.h>
#include <tdme/utils/ShortBuffer.h>
#include <tdme/engine/subsystems/renderer/fwd-tdme.h>
#include <tdme/gui/fwd-tdme.h>
#include <tdme/gui/nodes/fwd-tdme.h>
#include <tdme/gui/nodes/GUIColor.h>
#include <tdme/gui/renderer/fwd-tdme.h>
#include <tdme/math/Math.h>

using std::array;
using std::vector;

using tdme::utils::ByteBuffer;
using tdme::utils::FloatBuffer;
using tdme::utils::ShortBuffer;
using tdme::engine::subsystems::renderer::GLRenderer;
using tdme::gui::GUI;
using tdme::gui::nodes::GUIColor;
using tdme::gui::nodes::GUIScreenNode;
using tdme::math::Math;

/** 
 * GUI
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::gui::renderer::GUIRenderer final
{
	friend class GUIFont_CharacterDefinition;

private:
	static constexpr int32_t QUAD_COUNT { 1024 };
	static constexpr float SCREEN_LEFT { -1.0f };
	static constexpr float SCREEN_TOP { 1.0f };
	static constexpr float SCREEN_RIGHT { 1.0f };
	static constexpr float SCREEN_BOTTOM { -1.0f };

private:
	GUI* gui {  };
	GLRenderer* renderer {  };
	vector<int32_t>* vboIds {  };
	int32_t quadCount {  };
	ByteBuffer* sbIndicesByteBuffer {  };
	ByteBuffer* fbVerticesByteBuffer {  };
	FloatBuffer fbVertices {  };
	ByteBuffer* fbColorsByteBuffer {  };
	FloatBuffer fbColors {  };
	ByteBuffer* fbTextureCoordinatesByteBuffer {  };
	FloatBuffer fbTextureCoordinates {  };
	float renderAreaLeft {  };
	float renderAreaTop {  };
	float renderAreaRight {  };
	float renderAreaBottom {  };
	float renderOffsetX {  };
	float renderOffsetY {  };
	GUIScreenNode* screenNode {  };
	array<float, 4> fontColor {  };
	array<float, 4> effectColorMul {  };
	array<float, 4> effectColorAdd {  };
	array<float, 4> guiEffectColorMul {  };
	array<float, 4> guiEffectColorAdd {  };
	array<float, 4> effectColorMulFinal {  };
	array<float, 4> effectColorAddFinal {  };
	float guiEffectOffsetX {  };
	float guiEffectOffsetY {  };

public:

	/** 
	 * Set GUI
	 * @param gui
	 */
	void setGUI(GUI* gui);

	/** 
	 * @return GUI
	 */
	GUI* getGUI();

	/** 
	 * Init
	 */
	void initialize();

	/** 
	 * Dispose
	 */
	void dispose();

	/** 
	 * Init rendering
	 */
	void initRendering();

	/** 
	 * Done rendering
	 */
	void doneRendering();

	/** 
	 * Init screen
	 * @param screen node
	 */
	void initScreen(GUIScreenNode* screenNode);

	/** 
	 * Done screen
	 */
	void doneScreen();

	/** 
	 * Set effect color mul
	 * @param color
	 */
	void setFontColor(const GUIColor& color);

	/** 
	 * Set effect color mul
	 * @param color
	 */
	void setEffectColorMul(const GUIColor& color);

	/** 
	 * Set effect color add
	 * @param color
	 */
	void setEffectColorAdd(const GUIColor& color);

	/** 
	 * Set GUI effect color mul
	 * @param color
	 */
	void setGUIEffectColorMul(const GUIColor& color);

	/** 
	 * Set GUI effect color add
	 * @param color
	 */
	void setGUIEffectColorAdd(const GUIColor& color);

	/** 
	 * @return GUI effect offset X
	 */
	float getGuiEffectOffsetX();

	/** 
	 * Set GUI effect offset X
	 * @param gui effect offset X
	 */
	void setGUIEffectOffsetX(float guiEffectOffsetX);

	/** 
	 * @return GUI effect offset Y
	 */
	float getGuiEffectOffsetY();

	/** 
	 * Set GUI effect offset Y
	 * @param GUI effect offset Y
	 */
	void setGUIEffectOffsetY(float guiEffectOffsetY);

	/** 
	 * @return render area left
	 */
	inline float getRenderAreaLeft() {
		return renderAreaLeft;
	}

	/** 
	 * Set up render area left
	 * @param render area left
	 */
	inline void setRenderAreaLeft(float renderAreaLeft) {
		this->renderAreaLeft = renderAreaLeft;
	}

	/** 
	 * Set sub render area left
	 * @param render area left
	 */
	inline void setSubRenderAreaLeft(float renderAreaLeft) {
		this->renderAreaLeft = renderAreaLeft > this->renderAreaLeft ? renderAreaLeft : this->renderAreaLeft;
	}

	/** 
	 * @return render area top
	 */
	inline float getRenderAreaTop() {
		return renderAreaTop;
	}

	/** 
	 * Set up render area top
	 * @param render area top
	 */
	inline void setRenderAreaTop(float renderAreaTop) {
		this->renderAreaTop = renderAreaTop;
	}

	/** 
	 * Set sub render area top
	 * @param render area top
	 */
	inline void setSubRenderAreaTop(float renderAreaTop) {
		this->renderAreaTop = renderAreaTop < this->renderAreaTop ? renderAreaTop : this->renderAreaTop;
	}

	/** 
	 * @return render area right
	 */
	inline float getRenderAreaRight() {
		return renderAreaRight;
	}

	/** 
	 * Set up render area right
	 * @param render area right
	 */
	inline void setRenderAreaRight(float renderAreaRight) {
		this->renderAreaRight = renderAreaRight;
	}

	/** 
	 * Set sub render area right
	 * @param render area right
	 */
	inline void setSubRenderAreaRight(float renderAreaRight) {
		this->renderAreaRight = renderAreaRight < this->renderAreaRight ? renderAreaRight : this->renderAreaRight;
	}

	/** 
	 * @return render area bottom
	 */
	inline float getRenderAreaBottom() {
		return renderAreaBottom;
	}

	/** 
	 * Set up render area bottom
	 * @param render area bottom
	 */
	inline void setRenderAreaBottom(float renderAreaBottom) {
		this->renderAreaBottom = renderAreaBottom;
	}

	/** 
	 * Set sub render area bottom
	 * @param render area bottom
	 */
	inline void setSubRenderAreaBottom(float renderAreaBottom) {
		this->renderAreaBottom = renderAreaBottom > this->renderAreaBottom ? renderAreaBottom : this->renderAreaBottom;
	}

	/** 
	 * @return render offset x
	 */
	inline float getRenderOffsetX() {
		return renderOffsetX;
	}

	/** 
	 * Set render offset x
	 * @param render offset x
	 */
	inline void setRenderOffsetX(float renderOffsetX) {
		this->renderOffsetX = renderOffsetX;
	}

	/** 
	 * @return render offset y
	 */
	inline float getRenderOffsetY() {
		return renderOffsetY;
	}

	/** 
	 * Set render offset y
	 * @param render offset y
	 */
	inline void setRenderOffsetY(float renderOffsetY) {
		this->renderOffsetY = renderOffsetY;
	}

	/**
	 * @return if quad is visible
	 */
	inline bool isQuadVisible(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4) {
		x1 -= renderOffsetX;
		x2 -= renderOffsetX;
		x3 -= renderOffsetX;
		x4 -= renderOffsetX;
		y1 += renderOffsetY;
		y2 += renderOffsetY;
		y3 += renderOffsetY;
		y4 += renderOffsetY;
		x1 -= guiEffectOffsetX;
		x2 -= guiEffectOffsetX;
		x3 -= guiEffectOffsetX;
		x4 -= guiEffectOffsetX;
		y1 += guiEffectOffsetY;
		y2 += guiEffectOffsetY;
		y3 += guiEffectOffsetY;
		y4 += guiEffectOffsetY;
		auto renderAreaTop = this->renderAreaTop;
		auto renderAreaBottom = this->renderAreaBottom;
		auto renderAreaRight = this->renderAreaRight;
		auto renderAreaLeft = this->renderAreaLeft;
		/*
		// TODO: This prevented off screen rendering, but does collide with position effects and parent children viewport caches
		renderAreaTop = Math::min(renderAreaTop + guiEffectOffsetY, SCREEN_TOP);
		renderAreaBottom = Math::max(renderAreaBottom + guiEffectOffsetY, SCREEN_BOTTOM);
		renderAreaRight = Math::min(renderAreaRight - guiEffectOffsetX, SCREEN_RIGHT);
		renderAreaLeft = Math::max(renderAreaLeft - guiEffectOffsetX, SCREEN_LEFT);
		*/
		renderAreaTop = renderAreaTop + guiEffectOffsetY;
		renderAreaBottom = renderAreaBottom + guiEffectOffsetY;
		renderAreaRight = renderAreaRight - guiEffectOffsetX;
		renderAreaLeft = renderAreaLeft - guiEffectOffsetX;
		if (y3 > renderAreaTop) return false;
		if (y1 < renderAreaBottom) return false;
		if (x1 > renderAreaRight) return false;
		if (x2 < renderAreaLeft) return false;
		return true;
	}

	/** 
	 * Add quad
	 * Note: quad vertices order
	 * 1    2
	 * +----+
	 * |    |
	 * |    |
	 * +----+
	 * 4    3
	 * @param x 1
	 * @param y 1
	 * @param color red 1
	 * @param color green 1
	 * @param color blue 1
	 * @param color alpha 1
	 * @param texture u 1
	 * @param texture v 1
	 * @param x 2
	 * @param y 2
	 * @param color red 2
	 * @param color green 2
	 * @param color blue 2
	 * @param color alpha 2
	 * @param texture u 2
	 * @param texture v 2
	 * @param x 3
	 * @param y 3
	 * @param color red 3
	 * @param color green 3
	 * @param color blue 3
	 * @param color alpha 3
	 * @param texture u 3
	 * @param texture v 3
	 * @param x 4
	 * @param y 4
	 * @param color red 4
	 * @param color green 4
	 * @param color blue 4
	 * @param color alpha 4
	 * @param texture u 4
	 * @param texture v 4
	 */
	void addQuad(float x1, float y1, float colorR1, float colorG1, float colorB1, float colorA1, float tu1, float tv1, float x2, float y2, float colorR2, float colorG2, float colorB2, float colorA2, float tu2, float tv2, float x3, float y3, float colorR3, float colorG3, float colorB3, float colorA3, float tu3, float tv3, float x4, float y4, float colorR4, float colorG4, float colorB4, float colorA4, float tu4, float tv4);

	/** 
	 * Bind texture
	 * @param texture
	 */
	void bindTexture(int32_t textureId);

	/** 
	 * Render 
	 */
	void render();

	/**
	 * Public constructor
	 */
	GUIRenderer(GLRenderer* renderer);

	/**
	 * Destructor
	 */
	~GUIRenderer();
private:
	void init();
};
