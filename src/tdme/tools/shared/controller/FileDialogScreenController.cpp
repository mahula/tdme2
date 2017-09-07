// Generated from /tdme/src/tdme/tools/shared/controller/FileDialogScreenController.java
#include <tdme/tools/shared/controller/FileDialogScreenController.h>

#include <java/io/Serializable.h>
#include <java/lang/CharSequence.h>
#include <java/lang/Comparable.h>
#include <java/lang/String.h>
#include <tdme/gui/GUIParser.h>
#include <tdme/gui/events/Action.h>
#include <tdme/gui/events/GUIActionListener_Type.h>
#include <tdme/gui/nodes/GUIElementNode.h>
#include <tdme/gui/nodes/GUINode.h>
#include <tdme/gui/nodes/GUINodeController.h>
#include <tdme/gui/nodes/GUIParentNode.h>
#include <tdme/gui/nodes/GUIScreenNode.h>
#include <tdme/gui/nodes/GUITextNode.h>
#include <tdme/os/_FileSystem.h>
#include <tdme/os/_FileSystemInterface.h>
#include <tdme/tools/shared/controller/FileDialogScreenController_setupFileDialogListBox_1.h>
#include <tdme/utils/_Console.h>
#include <tdme/utils/_Exception.h>
#include <tdme/utils/MutableString.h>
#include <tdme/utils/StringUtils.h>
#include <SubArray.h>
#include <ObjectArray.h>

using tdme::tools::shared::controller::FileDialogScreenController;
using java::io::Serializable;
using java::lang::CharSequence;
using java::lang::Comparable;
using java::lang::String;
using tdme::gui::GUIParser;
using tdme::gui::events::Action;
using tdme::gui::events::GUIActionListener_Type;
using tdme::gui::nodes::GUIElementNode;
using tdme::gui::nodes::GUINode;
using tdme::gui::nodes::GUINodeController;
using tdme::gui::nodes::GUIParentNode;
using tdme::gui::nodes::GUIScreenNode;
using tdme::gui::nodes::GUITextNode;
using tdme::os::_FileSystem;
using tdme::os::_FileSystemInterface;
using tdme::tools::shared::controller::FileDialogScreenController_setupFileDialogListBox_1;
using tdme::utils::MutableString;
using tdme::utils::StringUtils;
using tdme::utils::_Console;
using tdme::utils::_Exception;

template<typename ComponentType, typename... Bases> struct SubArray;
namespace java {
namespace io {
typedef ::SubArray< ::java::io::Serializable, ::java::lang::ObjectArray > SerializableArray;
}  // namespace io

namespace lang {
typedef ::SubArray< ::java::lang::CharSequence, ObjectArray > CharSequenceArray;
typedef ::SubArray< ::java::lang::Comparable, ObjectArray > ComparableArray;
typedef ::SubArray< ::java::lang::String, ObjectArray, ::java::io::SerializableArray, ComparableArray, CharSequenceArray > StringArray;
}  // namespace lang
}  // namespace java

FileDialogScreenController::FileDialogScreenController() 
{
	this->cwd = _FileSystem::getInstance()->getCurrentWorkingPathName();
	this->value = new MutableString();
	this->applyAction = nullptr;
}

GUIScreenNode* FileDialogScreenController::getScreenNode()
{
	return screenNode;
}

const wstring& FileDialogScreenController::getPathName()
{
	return cwd;
}

const wstring& FileDialogScreenController::getFileName()
{
	return fileName->getController()->getValue()->toString()->getCPPWString();
}

void FileDialogScreenController::initialize()
{
	try {
		screenNode = GUIParser::parse(u"resources/tools/shared/gui"_j, u"filedialog.xml"_j);
		screenNode->setVisible(false);
		screenNode->addActionListener(this);
		screenNode->addChangeListener(this);
		caption = dynamic_cast< GUITextNode* >(screenNode->getNodeById(u"filedialog_caption"_j));
		files = dynamic_cast< GUIElementNode* >(screenNode->getNodeById(u"filedialog_files"_j));
		fileName = dynamic_cast< GUIElementNode* >(screenNode->getNodeById(u"filedialog_filename"_j));
	} catch (_Exception& exception) {
		_Console::print(string("FileDialogScreenController::initialize(): An error occurred: "));
		_Console::println(string(exception.what()));
	}
}

void FileDialogScreenController::dispose()
{
}

void FileDialogScreenController::setupFileDialogListBox()
{
	auto directory = cwd;
	if (directory.length() > 50) {
		directory = L"..." + StringUtils::substring(directory, directory.length() - 50 + 3);
	}

	caption->getText()->set(captionText)->append(directory);

	vector<wstring> fileList;
	try {
		auto directory = cwd;
		_FileSystem::getInstance()->list(directory, &fileList, new FileDialogScreenController_setupFileDialogListBox_1(this));
	} catch (_Exception& exception) {
		_Console::print(string("FileDialogScreenController::setupFileDialogListBox(): An error occurred: "));
		_Console::println(string(exception.what()));
	}

	auto filesInnerNode = dynamic_cast< GUIParentNode* >(files->getScreenNode()->getNodeById(new String(files->getId()->getCPPWString() + L"_inner")));
	auto idx = 1;
	wstring filesInnerNodeSubNodesXML = L"";
	filesInnerNodeSubNodesXML =
		filesInnerNodeSubNodesXML +
		L"<scrollarea width=\"100%\" height=\"100%\">\n";
	for (auto& file : fileList) {
		filesInnerNodeSubNodesXML =
			filesInnerNodeSubNodesXML +
			L"<selectbox-option text=\"" +
			GUIParser::escapeQuotes(file) +
			L"\" value=\"" +
			GUIParser::escapeQuotes(file) +
			L"\" />\n";
	}
	filesInnerNodeSubNodesXML =
		filesInnerNodeSubNodesXML + L"</scrollarea>\n";
	try {
		filesInnerNode->replaceSubNodes(new String(filesInnerNodeSubNodesXML), true);
	} catch (_Exception& exception) {
		_Console::print(string("FileDialogScreenController::setupFileDialogListBox(): An error occurred: "));
		_Console::println(string(exception.what()));
	}
}

void FileDialogScreenController::show(const wstring& cwd, const wstring& captionText, StringArray* extensions, const wstring& fileName, Action* applyAction)
{
	try {
		this->cwd = _FileSystem::getInstance()->getCanonicalPath(cwd, L"");
	} catch (_Exception& exception) {
		_Console::print(string("FileDialogScreenController::show(): An error occurred: "));
		_Console::println(string(exception.what()));
	}
	this->captionText = captionText;
	this->extensions = extensions;
	this->fileName->getController()->setValue(value->set(fileName));
	setupFileDialogListBox();
	screenNode->setVisible(true);
	this->applyAction = applyAction;
}

void FileDialogScreenController::close()
{
	screenNode->setVisible(false);
}

void FileDialogScreenController::onValueChanged(GUIElementNode* node)
{
	try {
		if (node->getId()->equals(files->getId()) == true) {
			auto selectedFile = node->getController()->getValue()->toString();
			if (_FileSystem::getInstance()->isPath(cwd + L"/" + selectedFile->getCPPWString()) == true) {
				try {
					cwd = _FileSystem::getInstance()->getCanonicalPath(cwd, selectedFile->getCPPWString());
				} catch (_Exception& exception) {
					_Console::print(string("FileDialogScreenController::onValueChanged(): An error occurred: "));
					_Console::println(string(exception.what()));
				}
				setupFileDialogListBox();
			} else {
				fileName->getController()->setValue(value->set(selectedFile));
			}
		}
	} catch (_Exception& exception) {
		_Console::print(string("FileDialogScreenController::onValueChanged(): An error occurred: "));
		_Console::println(string(exception.what()));
	}
}

void FileDialogScreenController::onActionPerformed(GUIActionListener_Type* type, GUIElementNode* node)
{
	{
		auto v = type;
		if ((v == GUIActionListener_Type::PERFORMED))
		{
			{
				if (node->getId()->equals(L"filedialog_apply")) {
					if (applyAction != nullptr)
						applyAction->performAction();

				} else if (node->getId()->equals(u"filedialog_abort"_j)) {
					close();
				}
				goto end_switch0;;
			}
		}
		if (((v == GUIActionListener_Type::PERFORMED) || ((v != GUIActionListener_Type::PERFORMED))))
		{
			{
				goto end_switch0;;
			}
		}
		end_switch0:;
	}

}
