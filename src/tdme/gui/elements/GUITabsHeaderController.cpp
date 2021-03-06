#include <tdme/gui/elements/GUITabsHeaderController.h>

#include <tdme/gui/GUI.h>
#include <tdme/gui/elements/GUITabController.h>
#include <tdme/gui/elements/GUITabsController.h>
#include <tdme/gui/events/GUIKeyboardEvent_Type.h>
#include <tdme/gui/events/GUIKeyboardEvent.h>
#include <tdme/gui/events/GUIMouseEvent_Type.h>
#include <tdme/gui/events/GUIMouseEvent.h>
#include <tdme/gui/nodes/GUIElementNode.h>
#include <tdme/gui/nodes/GUINode.h>
#include <tdme/gui/nodes/GUINodeController.h>
#include <tdme/gui/nodes/GUIParentNode.h>
#include <tdme/gui/nodes/GUIScreenNode.h>

using tdme::gui::elements::GUITabsHeaderController;
using tdme::gui::GUI;
using tdme::gui::elements::GUITabController;
using tdme::gui::elements::GUITabsController;
using tdme::gui::events::GUIKeyboardEvent_Type;
using tdme::gui::events::GUIKeyboardEvent;
using tdme::gui::events::GUIMouseEvent_Type;
using tdme::gui::events::GUIMouseEvent;
using tdme::gui::nodes::GUIElementNode;
using tdme::gui::nodes::GUINode;
using tdme::gui::nodes::GUINodeController;
using tdme::gui::nodes::GUIParentNode;
using tdme::gui::nodes::GUIScreenNode;

GUITabsHeaderController::GUITabsHeaderController(GUINode* node) 
	: GUINodeController(node)
{
	this->focus = false;
}

bool GUITabsHeaderController::isDisabled()
{
	return false;
}

void GUITabsHeaderController::setDisabled(bool disabled)
{
}

void GUITabsHeaderController::initialize()
{
	tabsNode = (dynamic_cast< GUIParentNode* >(node))->getParentControllerNode();
}

void GUITabsHeaderController::dispose()
{
}

void GUITabsHeaderController::postLayout()
{
}

bool GUITabsHeaderController::hasFocus()
{
	return focus;
}

void GUITabsHeaderController::unselect()
{
	(dynamic_cast< GUIParentNode* >(node))->getChildControllerNodes(childControllerNodes);
	for (auto i = 0; i < childControllerNodes.size(); i++) {
		auto childControllerNode = childControllerNodes[i];
		auto childController = childControllerNode->getController();
		if (dynamic_cast< GUITabController* >(childController) != nullptr) {
			auto guiTabController = dynamic_cast< GUITabController* >(childController);
			if (static_cast< GUINode* >(guiTabController->getNode()->getParentControllerNode()) != node)
				continue;

			guiTabController->setSelected(false);
		}
	}
}

void GUITabsHeaderController::determineTabControllers()
{
	tabControllers.clear();
	(dynamic_cast< GUIParentNode* >(node))->getChildControllerNodes(childControllerNodes);
	for (auto i = 0; i < childControllerNodes.size(); i++) {
		auto childControllerNode = childControllerNodes[i];
		auto childController = childControllerNode->getController();
		if (dynamic_cast< GUITabController* >(childController) != nullptr) {
			auto guiTabController = dynamic_cast< GUITabController* >(childController);
			if (static_cast< GUINode* >(guiTabController->getNode()->getParentControllerNode()) != node)
				continue;

			if (guiTabController->isDisabled() == true)
				continue;

			tabControllers.push_back(guiTabController);
		}
	}
}

int32_t GUITabsHeaderController::getSelectedTabIdx()
{
	auto tabControllerIdx = -1;
	for (auto i = 0; i < tabControllers.size(); i++) {
		auto tabController = tabControllers[i];
		if (tabController->isSelected() == true) {
			tabControllerIdx = i;
			break;
		}
	}
	return tabControllerIdx;
}

void GUITabsHeaderController::selectNext()
{
	determineTabControllers();
	auto tabControllerIdx = getSelectedTabIdx();
	unselect();
	tabControllerIdx = (tabControllerIdx + 1) % tabControllers.size();
	if (tabControllerIdx < 0)
		tabControllerIdx += tabControllers.size();

	tabControllers[tabControllerIdx]->setSelected(true);
	auto guiTabsController = dynamic_cast< GUITabsController* >(tabsNode->getController());
	guiTabsController->setTabContentSelected(tabControllers[tabControllerIdx]->getNode()->getId());
}

void GUITabsHeaderController::selectPrevious()
{
	determineTabControllers();
	auto tabControllerIdx = getSelectedTabIdx();
	unselect();
	tabControllerIdx = (tabControllerIdx - 1) % tabControllers.size();
	if (tabControllerIdx < 0)
		tabControllerIdx += tabControllers.size();

	tabControllers[tabControllerIdx]->setSelected(true);
	auto guiTabsController = dynamic_cast< GUITabsController* >(tabsNode->getController());
	guiTabsController->setTabContentSelected(tabControllers[tabControllerIdx]->getNode()->getId());
}

void GUITabsHeaderController::selectCurrent()
{
	determineTabControllers();
	auto tabControllerIdx = getSelectedTabIdx();
	unselect();
	tabControllers[tabControllerIdx]->setSelected(true);
	auto guiTabsController = dynamic_cast< GUITabsController* >(tabsNode->getController());
	guiTabsController->setTabContentSelected(tabControllers[tabControllerIdx]->getNode()->getId());
}

void GUITabsHeaderController::handleMouseEvent(GUINode* node, GUIMouseEvent* event)
{
	if (node == this->node && node->isEventBelongingToNode(event) && event->getButton() == MOUSE_BUTTON_LEFT) {
		event->setProcessed(true);
		if (event->getType() == GUIMouseEvent_Type::MOUSEEVENT_RELEASED) {
			node->getScreenNode()->getGUI()->setFoccussedNode(dynamic_cast< GUIElementNode* >(node));
		}
	}
}

void GUITabsHeaderController::handleKeyboardEvent(GUINode* node, GUIKeyboardEvent* event)
{
	if (node == this->node) {
		switch (event->getKeyCode()) {
		case GUIKeyboardEvent::KEYCODE_LEFT: {
				event->setProcessed(true);
				if (event->getType() == GUIKeyboardEvent_Type::KEYBOARDEVENT_KEY_PRESSED) {
					selectPrevious();
				}
			}
			break;
		case GUIKeyboardEvent::KEYCODE_RIGHT: {
				event->setProcessed(true);
				if (event->getType() == GUIKeyboardEvent_Type::KEYBOARDEVENT_KEY_PRESSED) {
					selectNext();
				}
			}
			break;
		}

	}
}

void GUITabsHeaderController::tick()
{
}

void GUITabsHeaderController::onFocusGained()
{
	focus = true;
	selectCurrent();
}

void GUITabsHeaderController::onFocusLost()
{
	focus = false;
	selectCurrent();
}

bool GUITabsHeaderController::hasValue()
{
	return false;
}

const MutableString& GUITabsHeaderController::getValue()
{
	return value;
}

void GUITabsHeaderController::setValue(const MutableString& value)
{
}

