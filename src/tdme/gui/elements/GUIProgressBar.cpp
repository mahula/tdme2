#include <tdme/gui/elements/GUIProgressBar.h>

#include <map>
#include <string>

#include <tdme/gui/elements/GUIProgressBarController.h>
#include <tdme/gui/nodes/GUIScreenNode.h>
#include <tdme/os/filesystem/FileSystem.h>
#include <tdme/os/filesystem/FileSystemException.h>
#include <tdme/os/filesystem/FileSystemInterface.h>

using std::map;
using std::string;

using tdme::gui::elements::GUIProgressBar;
using tdme::gui::elements::GUIProgressBarController;
using tdme::gui::nodes::GUIScreenNode;
using tdme::os::filesystem::FileSystem;
using tdme::os::filesystem::FileSystemException;
using tdme::os::filesystem::FileSystemInterface;

string GUIProgressBar::NAME = "progress-bar";

GUIProgressBar::GUIProgressBar()
{
	templateXML = FileSystem::getInstance()->getContentAsString("resources/gui-system/definitions/elements", "progressbar.xml");
}

const string& GUIProgressBar::getName()
{
	return NAME;
}

const string& GUIProgressBar::getTemplate()
{
	return templateXML;
}

map<string, string>& GUIProgressBar::getAttributes(GUIScreenNode* screenNode)
{
	attributes.clear();
	attributes["id"] = screenNode->allocateNodeId();
	attributes["disabled"] = "false";
	attributes["value"] = "0.0";
	attributes["width"] = "223"; // TODO: a.drewke
	attributes["height"] = "53";
	return attributes;
}

GUINodeController* GUIProgressBar::createController(GUINode* node)
{
	return new GUIProgressBarController(node);
}

