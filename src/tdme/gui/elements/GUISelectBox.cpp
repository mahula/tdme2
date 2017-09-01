// Generated from /tdme/src/tdme/gui/elements/GUISelectBox.java
#include <tdme/gui/elements/GUISelectBox.h>

#include <java/lang/String.h>
#include <tdme/gui/elements/GUISelectBoxController.h>
#include <tdme/gui/nodes/GUIScreenNode.h>
#include <tdme/os/_FileSystem.h>
#include <tdme/os/_FileSystemException.h>
#include <tdme/os/_FileSystemInterface.h>

using tdme::gui::elements::GUISelectBox;
using java::lang::String;
using tdme::gui::elements::GUISelectBoxController;
using tdme::gui::nodes::GUIScreenNode;
using tdme::os::_FileSystem;
using tdme::os::_FileSystemException;
using tdme::os::_FileSystemInterface;

GUISelectBox::GUISelectBox(const ::default_init_tag&)
	: super(*static_cast< ::default_init_tag* >(0))
{
	clinit();
}

GUISelectBox::GUISelectBox() throw (_FileSystemException)
	: GUISelectBox(*static_cast< ::default_init_tag* >(0))
{
	ctor();
}

String* GUISelectBox::NAME;

void GUISelectBox::ctor() throw (_FileSystemException)
{
	super::ctor();
	template_ = new String(_FileSystem::getInstance()->getContentAsString(L"resources/gui/definitions/elements", L"selectbox.xml"));
}

String* GUISelectBox::getName()
{
	return NAME;
}

String* GUISelectBox::getTemplate()
{
	return template_;
}

map<wstring, String*>* GUISelectBox::getAttributes(GUIScreenNode* screenNode)
{
	attributes.clear();
	attributes[L"id"] = screenNode->allocateNodeId();
	attributes[L"width"] = u"100%"_j;
	attributes[L"height"] = u"auto"_j;
	return &attributes;
}

GUINodeController* GUISelectBox::createController(GUINode* node)
{
	return new GUISelectBoxController(node);
}

extern java::lang::Class* class_(const char16_t* c, int n);

java::lang::Class* GUISelectBox::class_()
{
    static ::java::lang::Class* c = ::class_(u"tdme.gui.elements.GUISelectBox", 30);
    return c;
}

void GUISelectBox::clinit()
{
struct string_init_ {
	string_init_() {
	NAME = u"selectbox"_j;
	}
};

	static string_init_ string_init_instance;

	super::clinit();
}

java::lang::Class* GUISelectBox::getClass0()
{
	return class_();
}

