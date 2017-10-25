#pragma once

#include <string>
#include <vector>

#include <tdme.h>
#include <tdme/gui/elements/fwd-tdme.h>
#include <tdme/gui/events/fwd-tdme.h>
#include <tdme/gui/nodes/fwd-tdme.h>
#include <tdme/utils/fwd-tdme.h>
#include <tdme/gui/nodes/GUINodeController.h>

using std::vector;
using std::string;

using tdme::gui::nodes::GUINodeController;
using tdme::gui::events::GUIKeyboardEvent;
using tdme::gui::events::GUIMouseEvent;
using tdme::gui::nodes::GUINode;
using tdme::utils::MutableString;

/** 
 * GUI select box multiple controller
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::gui::elements::GUISelectBoxMultipleController final
	: public GUINodeController
{

private:
	static string CONDITION_DISABLED;
	static string CONDITION_ENABLED;
	static constexpr char VALUE_DELIMITER { L'|' };
	vector<GUINode*> childControllerNodes {  };
	vector<GUISelectBoxMultipleOptionController*> selectBoxMultipleOptionControllers {  };
	bool disabled {  };
	MutableString* value {  };
	MutableString* searchValue {  };

public:
	bool isDisabled() override;

	/** 
	 * Set disabled
	 * @param disabled
	 */
	void setDisabled(bool disabled) override;
	void initialize() override;
	void dispose() override;
	void postLayout() override;

public: /* protected */

	/** 
	 * Unselect all nodes
	 */
	void unselect();

	/** 
	 * Unfocus all nodes
	 */
	void unfocus();

private:

	/** 
	 * Determine select box option controllers
	 */
	void determineSelectBoxMultipleOptionControllers();

	/** 
	 * Get focussed option idx
	 */
	int32_t getFocussedOptionIdx();

public: /* protected */

	/** 
	 * Select current options
	 */
	void selectCurrent();

	/** 
	 * Focus next node
	 */
	void focusNext();

	/** 
	 * Focus previous
	 */
	void focusPrevious();

	/** 
	 * Toggle selected node
	 */
	void toggle();

public:
	void handleMouseEvent(GUINode* node, GUIMouseEvent* event) override;
	void handleKeyboardEvent(GUINode* node, GUIKeyboardEvent* event) override;
	void tick() override;
	void onFocusGained() override;
	void onFocusLost() override;
	bool hasValue() override;
	MutableString* getValue() override;
	void setValue(MutableString* value) override;

public: /* protected */
	GUISelectBoxMultipleController(GUINode* node);

private:
	void init();
};
