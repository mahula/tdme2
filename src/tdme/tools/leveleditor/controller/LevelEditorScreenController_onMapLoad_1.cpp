#include <tdme/tools/leveleditor/controller/LevelEditorScreenController_onMapLoad_1.h>

#include <tdme/tools/leveleditor/controller/LevelEditorScreenController.h>
#include <tdme/tools/leveleditor/views/LevelEditorView.h>
#include <tdme/tools/shared/controller/FileDialogPath.h>
#include <tdme/tools/shared/controller/FileDialogScreenController.h>
#include <tdme/tools/shared/views/PopUps.h>

using tdme::tools::leveleditor::controller::LevelEditorScreenController_onMapLoad_1;
using tdme::tools::leveleditor::controller::LevelEditorScreenController;
using tdme::tools::leveleditor::views::LevelEditorView;
using tdme::tools::shared::controller::FileDialogPath;
using tdme::tools::shared::controller::FileDialogScreenController;
using tdme::tools::shared::views::PopUps;

LevelEditorScreenController_onMapLoad_1::LevelEditorScreenController_onMapLoad_1(LevelEditorScreenController* levelEditorScreenController)
	: levelEditorScreenController(levelEditorScreenController)
{
}

void LevelEditorScreenController_onMapLoad_1::performAction()
{
	levelEditorScreenController->view->loadMap(levelEditorScreenController->view->getPopUps()->getFileDialogScreenController()->getPathName(), levelEditorScreenController->view->getPopUps()->getFileDialogScreenController()->getFileName());
	levelEditorScreenController->mapPath->setPath(levelEditorScreenController->view->getPopUps()->getFileDialogScreenController()->getPathName());
	levelEditorScreenController->view->getPopUps()->getFileDialogScreenController()->close();
}