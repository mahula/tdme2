#pragma once

#include <tdme.h>
#include <tdme/gui/events/fwd-tdme.h>
#include <tdme/gui/nodes/fwd-tdme.h>
#include <tdme/utils/fwd-tdme.h>

using tdme::gui::events::GUIKeyboardEvent;
using tdme::gui::events::GUIMouseEvent;
using tdme::gui::nodes::GUINode;
using tdme::utils::MutableString;


struct default_init_tag;

/** 
 * GUI element controller
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::gui::nodes::GUINodeController
{

public: /* protected */
	GUINode* node {  };
protected:

	/** 
	 * Constructor
	 * @param node
	 */
	void ctor(GUINode* node);

public:

	/** 
	 * @return node
	 */
	virtual GUINode* getNode();

	/** 
	 * Initialize controller after element has been created
	 */
	virtual void initialize() = 0;

	/** 
	 * Dispose controller
	 */
	virtual void dispose() = 0;

	/** 
	 * Post layout event
	 */
	virtual void postLayout() = 0;

	/** 
	 * Handle mouse event
	 * @param node
	 * @param event
	 */
	virtual void handleMouseEvent(GUINode* node, GUIMouseEvent* event) = 0;

	/** 
	 * Handle keyboard event
	 * @param node
	 * @param event
	 */
	virtual void handleKeyboardEvent(GUINode* node, GUIKeyboardEvent* event) = 0;

	/** 
	 * Tick method will be executed once per frame
	 */
	virtual void tick() = 0;

	/** 
	 * On focus gained
	 */
	virtual void onFocusGained() = 0;

	/** 
	 * On focus lost
	 */
	virtual void onFocusLost() = 0;

	/** 
	 * @return has value
	 */
	virtual bool hasValue() = 0;

	/** 
	 * @return value
	 */
	virtual MutableString* getValue() = 0;

	/** 
	 * Set value
	 * @param value
	 */
	virtual void setValue(MutableString* value) = 0;

	/** 
	 * @return is disabled
	 */
	virtual bool isDisabled() = 0;

	/** 
	 * Set disabled 
	 * @param disabled
	 */
	virtual void setDisabled(bool disabled) = 0;

	// Generated

public: /* protected */
	GUINodeController(GUINode* node);
protected:
	GUINodeController(const ::default_init_tag&);
};
