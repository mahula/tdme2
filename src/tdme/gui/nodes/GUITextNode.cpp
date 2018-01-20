#include <tdme/gui/nodes/GUITextNode.h>

#include <string>
#include <vector>

#include <tdme/gui/GUI.h>
#include <tdme/gui/nodes/GUIColor.h>
#include <tdme/gui/nodes/GUINode_Border.h>
#include <tdme/gui/nodes/GUINode_ComputedConstraints.h>
#include <tdme/gui/nodes/GUINode_Padding.h>
#include <tdme/gui/renderer/GUIFont.h>
#include <tdme/utils/MutableString.h>

using std::vector;
using std::string;

using tdme::gui::nodes::GUITextNode;
using tdme::gui::GUI;
using tdme::gui::nodes::GUIColor;
using tdme::gui::nodes::GUINode_Border;
using tdme::gui::nodes::GUINode_ComputedConstraints;
using tdme::gui::nodes::GUINode_Padding;
using tdme::gui::renderer::GUIFont;
using tdme::utils::MutableString;

GUITextNode::GUITextNode(GUIScreenNode* screenNode, GUIParentNode* parentNode, const string& id, GUINode_Flow* flow, GUINode_Alignments* alignments, GUINode_RequestedConstraints* requestedConstraints, GUIColor* backgroundColor, GUINode_Border* border, GUINode_Padding* padding, GUINodeConditions* showOn, GUINodeConditions* hideOn, const string& font, const string& color, MutableString* text)  /* throws(Exception) */
	: 	GUINode(screenNode, parentNode, id, flow, alignments, requestedConstraints, backgroundColor, border, padding, showOn, hideOn)
{
	this->font = GUI::getFont(font);
	this->color = color.empty() == true || color.length() == 0 ? new GUIColor() : new GUIColor(color);
	this->text = text;
	this->font->initialize();
}

const string GUITextNode::getNodeType()
{
	return "text";
}

bool GUITextNode::isContentNode()
{
	return true;
}

int32_t GUITextNode::getContentWidth()
{
	return font->getTextWidth(text) + border->left + border->right+ padding->left+ padding->right;
}

int32_t GUITextNode::getContentHeight()
{
	return font->getLineHeight() + border->top + border->bottom+ padding->top+ padding->bottom;
}

MutableString* GUITextNode::getText()
{
	return text;
}

void GUITextNode::dispose()
{
	this->font->dispose();
	GUINode::dispose();
}

void GUITextNode::render(GUIRenderer* guiRenderer, vector<GUINode*>* floatingNodes)
{
	if (conditionsMet == false)
		return;

	GUINode::render(guiRenderer, floatingNodes);
	font->drawString(guiRenderer, computedConstraints.left + computedConstraints.alignmentLeft + computedConstraints.contentAlignmentLeft, computedConstraints.top + computedConstraints.alignmentTop + computedConstraints.contentAlignmentTop, text, 0, 0, color);
}

