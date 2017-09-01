// Generated from /tdme/src/tdme/gui/elements/GUISelectBoxOption.java
#include <tdme/gui/elements/GUISelectBoxOption.h>

#include <java/lang/String.h>
#include <tdme/gui/elements/GUISelectBoxOptionController.h>
#include <tdme/gui/nodes/GUIScreenNode.h>
#include <tdme/os/_FileSystem.h>
#include <tdme/os/_FileSystemException.h>
#include <tdme/os/_FileSystemInterface.h>

using tdme::gui::elements::GUISelectBoxOption;
using java::lang::String;
using tdme::gui::elements::GUISelectBoxOptionController;
using tdme::gui::nodes::GUIScreenNode;
using tdme::os::_FileSystem;
using tdme::os::_FileSystemException;
using tdme::os::_FileSystemInterface;

GUISelectBoxOption::GUISelectBoxOption(const ::default_init_tag&)
	: super(*static_cast< ::default_init_tag* >(0))
{
	clinit();
}

GUISelectBoxOption::GUISelectBoxOption() throw (_FileSystemException)
	: GUISelectBoxOption(*static_cast< ::default_init_tag* >(0))
{
	ctor();
}

String* GUISelectBoxOption::NAME;

void GUISelectBoxOption::ctor() throw (_FileSystemException)
{
	super::ctor();
	template_ = new String(_FileSystem::getInstance()->getContentAsString(L"resources/gui/definitions/elements", L"selectbox-option.xml"));
}

String* GUISelectBoxOption::getName()
{
	return NAME;
}

String* GUISelectBoxOption::getTemplate()
{
	return template_;
}

map<wstring, String*>* GUISelectBoxOption::getAttributes(GUIScreenNode* screenNode)
{
	attributes.clear();
	attributes[L"id"] = screenNode->allocateNodeId();
	return &attributes;
}

GUINodeController* GUISelectBoxOption::createController(GUINode* node)
{
	return new GUISelectBoxOptionController(node);
}

extern java::lang::Class* class_(const char16_t* c, int n);

java::lang::Class* GUISelectBoxOption::class_()
{
    static ::java::lang::Class* c = ::class_(u"tdme.gui.elements.GUISelectBoxOption", 36);
    return c;
}

void GUISelectBoxOption::clinit()
{
struct string_init_ {
	string_init_() {
	NAME = u"selectbox-option"_j;
	}
};

	static string_init_ string_init_instance;

	super::clinit();
}

java::lang::Class* GUISelectBoxOption::getClass0()
{
	return class_();
}

