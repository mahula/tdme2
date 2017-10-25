#include <tdme/gui/elements/GUISelectBoxOption.h>

#include <tdme/gui/elements/GUISelectBoxOptionController.h>
#include <tdme/gui/nodes/GUIScreenNode.h>
#include <tdme/os/filesystem/FileSystem.h>
#include <tdme/os/filesystem/FileSystemException.h>
#include <tdme/os/filesystem/FileSystemInterface.h>

using tdme::gui::elements::GUISelectBoxOption;
using tdme::gui::elements::GUISelectBoxOptionController;
using tdme::gui::nodes::GUIScreenNode;
using tdme::os::filesystem::FileSystem;
using tdme::os::filesystem::FileSystemException;
using tdme::os::filesystem::FileSystemInterface;

GUISelectBoxOption::GUISelectBoxOption() throw (FileSystemException)
{
	template_ = FileSystem::getInstance()->getContentAsString("resources/gui/definitions/elements", "selectbox-option.xml");
}

string GUISelectBoxOption::NAME = "selectbox-option";

const string& GUISelectBoxOption::getName()
{
	return NAME;
}

const string& GUISelectBoxOption::getTemplate()
{
	return template_;
}

map<string, string>* GUISelectBoxOption::getAttributes(GUIScreenNode* screenNode)
{
	attributes.clear();
	attributes["id"] = screenNode->allocateNodeId();
	return &attributes;
}

GUINodeController* GUISelectBoxOption::createController(GUINode* node)
{
	return new GUISelectBoxOptionController(node);
}

