#include <tdme/gui/nodes/GUIScreenNode.h>

#include <algorithm>
#include <set>
#include <map>
#include <string>

#include <tdme/gui/GUI.h>
#include <tdme/gui/effects/GUIEffect.h>
#include <tdme/gui/events/GUIActionListener.h>
#include <tdme/gui/events/GUIChangeListener.h>
#include <tdme/gui/events/GUIFocusListener.h>
#include <tdme/gui/events/GUIInputEventHandler.h>
#include <tdme/gui/events/GUIMouseOverListener.h>
#include <tdme/gui/nodes/GUIElementNode.h>
#include <tdme/gui/nodes/GUIElementController.h>
#include <tdme/gui/nodes/GUINode_ComputedConstraints.h>
#include <tdme/gui/nodes/GUINode_RequestedConstraints_RequestedConstraintsType.h>
#include <tdme/gui/nodes/GUINode_RequestedConstraints.h>
#include <tdme/gui/nodes/GUINode_Scale9Grid.h>
#include <tdme/gui/nodes/GUINode.h>
#include <tdme/gui/nodes/GUINodeController.h>
#include <tdme/gui/nodes/GUIParentNode.h>
#include <tdme/gui/nodes/GUIScreenNode_SizeConstraints.h>
#include <tdme/gui/renderer/GUIRenderer.h>
#include <tdme/utils/Console.h>
#include <tdme/utils/Integer.h>
#include <tdme/utils/MutableString.h>

using std::map;
using std::remove;
using std::set;
using std::string;
using std::to_string;

using tdme::gui::nodes::GUIScreenNode;
using tdme::gui::GUI;
using tdme::gui::effects::GUIEffect;
using tdme::gui::events::GUIActionListener;
using tdme::gui::events::GUIChangeListener;
using tdme::gui::events::GUIInputEventHandler;
using tdme::gui::events::GUIMouseOverListener;
using tdme::gui::nodes::GUIElementNode;
using tdme::gui::nodes::GUIElementController;
using tdme::gui::nodes::GUINode_ComputedConstraints;
using tdme::gui::nodes::GUINode_RequestedConstraints_RequestedConstraintsType;
using tdme::gui::nodes::GUINode_RequestedConstraints;
using tdme::gui::nodes::GUINode_Scale9Grid;
using tdme::gui::nodes::GUINode;
using tdme::gui::nodes::GUINodeController;
using tdme::gui::nodes::GUIParentNode;
using tdme::gui::nodes::GUIScreenNode_SizeConstraints;
using tdme::gui::renderer::GUIRenderer;
using tdme::utils::Console;
using tdme::utils::Integer;
using tdme::utils::MutableString;

GUIScreenNode::GUIScreenNode(
	const string& id,
	GUINode_Flow* flow,
	GUIParentNode_Overflow* overflowX,
	GUIParentNode_Overflow* overflowY,
	const GUINode_Alignments& alignments,
	const GUINode_RequestedConstraints& requestedConstraints,
	const GUIColor& backgroundColor,
	const string& backgroundImage,
	const GUINode_Scale9Grid& backgroundImageScale9Grid,
	const GUIColor& backgroundImageEffectColorMul,
	const GUIColor& backgroundImageEffectColorAdd,
	const GUINode_Border& border,
	const GUINode_Padding& padding,
	const GUIScreenNode_SizeConstraints& sizeConstraints,
	const GUINodeConditions& showOn,
	const GUINodeConditions& hideOn,
	bool scrollable,
	bool popUp
):
	GUIParentNode(nullptr, nullptr, id, flow, overflowX, overflowY, alignments, requestedConstraints, backgroundColor, backgroundImage, backgroundImageScale9Grid, backgroundImageEffectColorMul, backgroundImageEffectColorAdd, border, padding, showOn, hideOn)
{
	this->sizeConstraints = sizeConstraints;
	this->gui = nullptr;
	this->nodeCounter = 0;
	this->screenWidth = 0;
	this->screenHeight = 0;
	this->inputEventHandler = nullptr;
	this->screenNode = this;
	this->parentNode = nullptr;
	this->visible = true;
	this->popUp = popUp;
	this->guiEffectOffsetX = 0;
	this->guiEffectOffsetY = 0;
	this->reshapeRequested = false;

}

GUIScreenNode::~GUIScreenNode() {
	// remove sub nodes
	for (auto i = 0; i < subNodes.size(); i++) {
		removeNode(subNodes[i]);
	}
	subNodes.clear();

	// dispose
	GUINode::dispose();

	// remove effects
	vector<string> effectsToRemove;
	for (auto effectIt: effects) {
		effectsToRemove.push_back(effectIt.first);
	}
	for (auto effectToRemoveId: effectsToRemove) {
		removeEffect(effectToRemoveId);
	}
}

GUI* GUIScreenNode::getGUI()
{
	return gui;
}

void GUIScreenNode::setGUI(GUI* gui)
{
	this->gui = gui;
}

void GUIScreenNode::setVisible(bool visible)
{
	this->visible = visible;
	if (gui != nullptr)
		gui->invalidateFocussedNode();

}

void GUIScreenNode::setPopUp(bool popUp)
{
	this->popUp = popUp;
}

const vector<GUINode*>& GUIScreenNode::getFloatingNodes()
{
	return floatingNodes;
}

int32_t GUIScreenNode::getGUIEffectOffsetX()
{
	return guiEffectOffsetX;
}

void GUIScreenNode::setGUIEffectOffsetX(int32_t guiEffectOffsetX)
{
	this->guiEffectOffsetX = guiEffectOffsetX;
}

int32_t GUIScreenNode::getGUIEffectOffsetY()
{
	return guiEffectOffsetY;
}

void GUIScreenNode::setGUIEffectOffsetY(int32_t guiEffectOffsetY)
{
	this->guiEffectOffsetY = guiEffectOffsetY;
}

bool GUIScreenNode::isContentNode()
{
	return false;
}

int32_t GUIScreenNode::getContentWidth()
{
	return -1;
}

int32_t GUIScreenNode::getContentHeight()
{
	return -1;
}

void GUIScreenNode::layout()
{
	for (auto i = 0; i < subNodes.size(); i++) {
		subNodes[i]->layout();
	}
	getChildControllerNodes(childControllerNodes);
	for (auto i = 0; i < childControllerNodes.size(); i++) {
		auto node = childControllerNodes[i];
		auto controller = node->getController();
		if (controller != nullptr && node->layouted == true) {
			controller->postLayout();
		}
	}
	layouted = true;
}

void GUIScreenNode::layout(GUINode* node)
{
	if (dynamic_cast< GUIParentNode* >(node) != nullptr) {
		auto parentNode = dynamic_cast< GUIParentNode* >(node);
		if (parentNode->conditionsMet == true) {
			parentNode->layoutSubNodes();
			parentNode->layoutSubNodes();
			parentNode->getChildControllerNodes(childControllerNodes);
			for (auto i = 0; i < childControllerNodes.size(); i++) {
				auto childNode = childControllerNodes[i];
				auto controller = childNode->getController();
				if (controller != nullptr && childNode->layouted == true) controller->postLayout();
			}
			auto controller = parentNode->getController();
			if (controller != nullptr) controller->postLayout();
			parentNode->layouted = true;
		} else {
			parentNode->layouted = false;
		}
	} else {
		if (node->conditionsMet == true) {
			node->computeContentAlignment();
			node->layouted = true;
			auto controller = node->getController();
			if (controller != nullptr) controller->postLayout();
		} else {
			node->layouted = false;
		}
	}
}

void GUIScreenNode::setScreenSize(int32_t width, int32_t height)
{
	this->screenWidth = width;
	this->screenHeight = height;
	this->requestedConstraints.widthType = GUINode_RequestedConstraints_RequestedConstraintsType::PIXEL;
	this->requestedConstraints.width = width;
	this->requestedConstraints.heightType = GUINode_RequestedConstraints_RequestedConstraintsType::PIXEL;
	this->requestedConstraints.height = height;
	this->computedConstraints.left = 0;
	this->computedConstraints.top = 0;
	this->computedConstraints.width = width;
	this->computedConstraints.height = height;
	layouted = false;
}

const string GUIScreenNode::getNodeType()
{
	return "screen";
}

GUINode* GUIScreenNode::getNodeById(const string& nodeId)
{
	auto nodesByIdIt = nodesById.find(nodeId);
	if (nodesByIdIt == nodesById.end()) {
		return nullptr;
	}
	return nodesByIdIt->second;
}

const string GUIScreenNode::allocateNodeId()
{
	return "tdme_gui_anonymous_node_" + to_string(nodeCounter++);
}

bool GUIScreenNode::addNode(GUINode* node)
{
	// allocate node id if not given
	if (node->id.length() == 0) {
		node->id = allocateNodeId();
	}

	// if node does exist do not insert it and return
	if (nodesById.find(node->id) != nodesById.end()) {
		return false;
	}
	// otherwise go
	nodesById[node->id] = node;

	// add to floating nodes
	if (node->flow == GUINode_Flow::FLOATING) floatingNodes.push_back(node);

	return true;
}

bool GUIScreenNode::removeNode(GUINode* node)
{
	if (dynamic_cast< GUIParentNode* >(node) != nullptr) {
		auto parentNode = dynamic_cast< GUIParentNode* >(node);
		for (auto i = 0; i < parentNode->subNodes.size(); i++) {
			removeNode(parentNode->subNodes[i]);
		}
		parentNode->subNodes.clear();
	}
	nodesById.erase(node->id);
	tickNodesById.erase(node->getId());
	floatingNodes.erase(remove(floatingNodes.begin(), floatingNodes.end(), node), floatingNodes.end());
	node->dispose();
	delete node;
	return true;
}

void GUIScreenNode::render(GUIRenderer* guiRenderer)
{
	guiRenderer->initScreen(this);
	for (const auto& i : effects) {
		auto effect = i.second;
		if (effect->isActive() == true) {
			effect->update(guiRenderer);
			effect->apply(guiRenderer);
		}
	}
	GUIParentNode::layoutOnDemand();
	GUIParentNode::render(guiRenderer);
	guiRenderer->doneScreen();
}

void GUIScreenNode::renderFloatingNodes(GUIRenderer* guiRenderer)
{
	guiRenderer->initScreen(this);
	for (auto& i : effects) {
		auto effect = i.second;
		effect->apply(guiRenderer);
	}
	for (auto i = 0; i < floatingNodes.size(); i++) {
		guiRenderer->setRenderAreaLeft(GUIRenderer::SCREEN_LEFT);
		guiRenderer->setRenderAreaTop(GUIRenderer::SCREEN_TOP);
		guiRenderer->setRenderAreaRight(GUIRenderer::SCREEN_RIGHT);
		guiRenderer->setRenderAreaBottom(GUIRenderer::SCREEN_BOTTOM);
		floatingNodes[i]->render(guiRenderer);
	}
	guiRenderer->doneScreen();
}

void GUIScreenNode::determineFocussedNodes(GUIParentNode* parentNode, vector<GUIElementNode*>& focusableNodes)
{
	if (parentNode->conditionsMet == false) {
		return;
	}
	if (dynamic_cast< GUIElementNode* >(parentNode) != nullptr) {
		auto parentElementNode = dynamic_cast< GUIElementNode* >(parentNode);
		if (parentElementNode->focusable == true && (parentElementNode->getController() == nullptr || parentElementNode->getController()->isDisabled() == false)) {
			focusableNodes.push_back(dynamic_cast< GUIElementNode* >(parentNode));
		}
	}
	for (auto i = 0; i < parentNode->subNodes.size(); i++) {
		auto subNode = parentNode->subNodes[i];
		if (dynamic_cast< GUIParentNode* >(subNode) != nullptr) {
			determineFocussedNodes(dynamic_cast< GUIParentNode* >(subNode), focusableNodes);
		}
	}
}

void GUIScreenNode::determineMouseEventNodes(GUIMouseEvent* event, bool floatingNode, set<string>& eventNodeIds, set<string>& eventFloatingNodeIds)
{
	for (auto i = 0; i < floatingNodes.size(); i++) {
		floatingNodes[i]->determineMouseEventNodes(event, floatingNode == true || flow == GUINode_Flow::FLOATING, eventNodeIds, eventFloatingNodeIds);
	}
	GUIParentNode::determineMouseEventNodes(event, floatingNode, eventNodeIds, eventFloatingNodeIds);
}

void GUIScreenNode::handleKeyboardEvent(GUIKeyboardEvent* event)
{
}

void GUIScreenNode::addActionListener(GUIActionListener* listener)
{
	removeActionListener(listener);
	actionListener.push_back(listener);
}

void GUIScreenNode::removeActionListener(GUIActionListener* listener)
{
	actionListener.erase(std::remove(actionListener.begin(), actionListener.end(), listener), actionListener.end());
}

GUIInputEventHandler* GUIScreenNode::getInputEventHandler()
{
	return inputEventHandler;
}

void GUIScreenNode::setInputEventHandler(GUIInputEventHandler* inputEventHandler)
{
	this->inputEventHandler = inputEventHandler;
}

void GUIScreenNode::delegateActionPerformed(GUIActionListener_Type* type, GUIElementNode* node)
{
	for (auto i = 0; i < actionListener.size(); i++) {
		actionListener[i]->onActionPerformed(type, node);
	}
}

void GUIScreenNode::addChangeListener(GUIChangeListener* listener)
{
	removeChangeListener(listener);
	changeListener.push_back(listener);
}

void GUIScreenNode::removeChangeListener(GUIChangeListener* listener)
{
	changeListener.erase(std::remove(changeListener.begin(), changeListener.end(), listener), changeListener.end());
}

void GUIScreenNode::delegateValueChanged(GUIElementNode* node)
{
	node->executeOnChangeExpression();
	for (auto i = 0; i < changeListener.size(); i++) {
		changeListener[i]->onValueChanged(node);
	}
}

void GUIScreenNode::addMouseOverListener(GUIMouseOverListener* listener)
{
	removeMouseOverListener(listener);
	mouseOverListener.push_back(listener);
}

void GUIScreenNode::removeMouseOverListener(GUIMouseOverListener* listener)
{
	mouseOverListener.erase(std::remove(mouseOverListener.begin(), mouseOverListener.end(), listener), mouseOverListener.end());
}

void GUIScreenNode::delegateMouseOver(GUIElementNode* node)
{
	for (auto i = 0; i < mouseOverListener.size(); i++) {
		mouseOverListener[i]->onMouseOver(node);
	}
}

void GUIScreenNode::addFocusListener(GUIFocusListener* listener)
{
	removeFocusListener(listener);
	focusListener.push_back(listener);
}

void GUIScreenNode::removeFocusListener(GUIFocusListener* listener)
{
	focusListener.erase(std::remove(focusListener.begin(), focusListener.end(), listener), focusListener.end());
}

void GUIScreenNode::delegateFocus(GUIElementNode* node) {
	for (auto i = 0; i < focusListener.size(); i++) {
		focusListener[i]->onFocus(node);
	}
}

void GUIScreenNode::delegateUnfocus(GUIElementNode* node) {
	for (auto i = 0; i < focusListener.size(); i++) {
		focusListener[i]->onUnfocus(node);
	}
}

void GUIScreenNode::addTickNode(GUINode* node) {
	tickNodesById[node->getId()] = node;
}

void GUIScreenNode::removeTickNode(GUINode* node) {
	tickNodesById.erase(node->getId());
}

void GUIScreenNode::tick() {
	auto _tickNodesById = tickNodesById;
	for (auto tickNodesByIdIt: _tickNodesById) {
		auto node = tickNodesByIdIt.second;
		if (node->controller != nullptr) node->controller->tick();
	}
}

void GUIScreenNode::getValues(map<string, MutableString>& values)
{
	values.clear();
	getChildControllerNodes(childControllerNodes);
	for (auto i = 0; i < childControllerNodes.size(); i++) {
		auto childControllerNode = childControllerNodes[i];
		if (dynamic_cast< GUIElementNode* >(childControllerNode) != nullptr == false)
			continue;

		auto guiElementNode = (dynamic_cast< GUIElementNode* >(childControllerNode));
		auto guiElementNodeController = guiElementNode->getController();
		if (guiElementNodeController->hasValue()) {
			auto& name = guiElementNode->getName();
			auto& value = guiElementNodeController->getValue();
			auto currentValueIt = values.find(name);
			if (currentValueIt == values.end() || currentValueIt->second.length() == 0) {
				values[name] = value;
			}
		}
	}
}

void GUIScreenNode::setValues(const map<string, MutableString>& values)
{
	getChildControllerNodes(childControllerNodes);
	for (auto i = 0; i < childControllerNodes.size(); i++) {
		auto childControllerNode = childControllerNodes[i];
		if (dynamic_cast< GUIElementNode* >(childControllerNode) != nullptr == false)
			continue;

		auto guiElementNode = (dynamic_cast< GUIElementNode* >(childControllerNode));
		auto guiElementNodeController = guiElementNode->getController();
		if (guiElementNodeController->hasValue()) {
			auto name = guiElementNode->getName();
			auto newValueIt = values.find(name);
			if (newValueIt == values.end())
				continue;
			guiElementNodeController->setValue(newValueIt->second);
		}
	}
}

bool GUIScreenNode::addEffect(const string& id, GUIEffect* effect)
{
	if (effects.find(id) != effects.end()) {
		return false;
	}
	effects[id] = effect;
	return true;
}

GUIEffect* GUIScreenNode::getEffect(const string& id)
{
	auto effectIt = effects.find(id);
	if (effectIt == effects.end()) {
		return nullptr;
	}
	return effectIt->second;
}

bool GUIScreenNode::removeEffect(const string& id)
{
	auto effectIt = effects.find(id);
	if (effectIt == effects.end()) {
		return false;
	}
	delete effectIt->second;
	effects.erase(effectIt);
	return true;
}

GUIScreenNode_SizeConstraints GUIScreenNode::createSizeConstraints(const string& minWidth, const string& minHeight, const string& maxWidth, const string& maxHeight)
{
	GUIScreenNode_SizeConstraints constraints;
	constraints.minWidth = minWidth.empty() == true?-1:Integer::parseInt(minWidth);
	constraints.minHeight = minHeight.empty() == true?-1:Integer::parseInt(minHeight);
	constraints.maxWidth = maxWidth.empty() == true?-1:Integer::parseInt(maxWidth);
	constraints.maxHeight = maxHeight.empty() == true?-1:Integer::parseInt(maxHeight);
	return constraints;
}
