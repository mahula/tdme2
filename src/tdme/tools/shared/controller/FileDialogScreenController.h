
#pragma once

#include <string>
#include <vector>

#include <tdme/gui/events/fwd-tdme.h>
#include <tdme/gui/nodes/fwd-tdme.h>
#include <tdme/tools/shared/controller/fwd-tdme.h>
#include <tdme/utils/fwd-tdme.h>
#include <tdme/tools/shared/controller/ScreenController.h>
#include <tdme/gui/events/GUIActionListener.h>
#include <tdme/gui/events/GUIChangeListener.h>

using std::vector;
using std::wstring;

using tdme::tools::shared::controller::ScreenController;
using tdme::gui::events::GUIActionListener;
using tdme::gui::events::GUIChangeListener;
using tdme::gui::events::Action;
using tdme::gui::events::GUIActionListener_Type;
using tdme::gui::nodes::GUIElementNode;
using tdme::gui::nodes::GUIScreenNode;
using tdme::gui::nodes::GUITextNode;
using tdme::utils::MutableString;

/** 
 * File dialog screen controller
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::tools::shared::controller::FileDialogScreenController
	: public ScreenController
	, public virtual GUIActionListener
	, public virtual GUIChangeListener
{
	friend class FileDialogScreenController_setupFileDialogListBox_1;

private:
	GUIScreenNode* screenNode {  };
	wstring cwd {  };
	vector<wstring> extensions {  };
	wstring captionText {  };
	GUITextNode* caption {  };
	GUIElementNode* fileName {  };
	GUIElementNode* files {  };
	MutableString* value {  };
	Action* applyAction {  };

public:
	GUIScreenNode* getScreenNode() override;

	/** 
	 * @return path name
	 */
	virtual const wstring& getPathName();

	/** 
	 * @return file name
	 */
	virtual const wstring getFileName();

	void initialize() override;
	void dispose() override;

private:

	/** 
	 * Set up file dialog list box
	 */
	void setupFileDialogListBox();

public:

	/** 
	 * Shows the file dialog pop up
	 * @param caption text
	 * @param extensions
	 * @param apply action
	 * @throws IOException 
	 */
	virtual void show(const wstring& cwd, const wstring& captionText, vector<wstring>* extensions, const wstring& fileName, Action* applyAction);

	/** 
	 * Abort the file dialog pop up
	 */
	virtual void close();

	/** 
	 * On value changed
	 */
	void onValueChanged(GUIElementNode* node) override;
	void onActionPerformed(GUIActionListener_Type* type, GUIElementNode* node) override;

	/**
	 * Public constructor
	 */
	FileDialogScreenController();
};
