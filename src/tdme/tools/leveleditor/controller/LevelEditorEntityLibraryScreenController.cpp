#include <tdme/tools/leveleditor/controller/LevelEditorEntityLibraryScreenController.h>

#include <string>

#include <tdme/engine/fileio/models/ModelReader.h>
#include <tdme/gui/GUIParser.h>
#include <tdme/gui/events/GUIActionListener_Type.h>
#include <tdme/gui/nodes/GUIElementNode.h>
#include <tdme/gui/nodes/GUINode.h>
#include <tdme/gui/nodes/GUINodeController.h>
#include <tdme/gui/nodes/GUIParentNode.h>
#include <tdme/gui/nodes/GUIScreenNode.h>
#include <tdme/tools/leveleditor/TDMELevelEditor.h>
#include <tdme/tools/leveleditor/controller/LevelEditorEntityLibraryScreenController_onValueChanged_1.h>
#include <tdme/tools/leveleditor/views/EmptyView.h>
#include <tdme/tools/leveleditor/views/LevelEditorView.h>
#include <tdme/tools/leveleditor/views/ModelEditorView.h>
#include <tdme/tools/leveleditor/views/ParticleSystemView.h>
#include <tdme/tools/leveleditor/views/TriggerView.h>
#include <tdme/tools/shared/controller/FileDialogScreenController.h>
#include <tdme/tools/shared/controller/InfoDialogScreenController.h>
#include <tdme/tools/shared/model/LevelEditorEntity_EntityType.h>
#include <tdme/tools/shared/model/LevelEditorEntity.h>
#include <tdme/tools/shared/model/LevelEditorEntityLibrary.h>
#include <tdme/tools/shared/model/LevelEditorLevel.h>
#include <tdme/tools/shared/tools/Tools.h>
#include <tdme/tools/shared/views/PopUps.h>
#include <tdme/tools/shared/views/View.h>
#include <tdme/utils/MutableString.h>
#include <tdme/utils/Console.h>
#include <tdme/utils/Exception.h>

using std::string;
using std::to_string;

using tdme::tools::leveleditor::controller::LevelEditorEntityLibraryScreenController;
using tdme::engine::fileio::models::ModelReader;
using tdme::gui::GUIParser;
using tdme::gui::events::GUIActionListener_Type;
using tdme::gui::nodes::GUIElementNode;
using tdme::gui::nodes::GUINode;
using tdme::gui::nodes::GUINodeController;
using tdme::gui::nodes::GUIParentNode;
using tdme::gui::nodes::GUIScreenNode;
using tdme::tools::leveleditor::TDMELevelEditor;
using tdme::tools::leveleditor::controller::LevelEditorEntityLibraryScreenController_onValueChanged_1;
using tdme::tools::leveleditor::views::EmptyView;
using tdme::tools::leveleditor::views::LevelEditorView;
using tdme::tools::leveleditor::views::ModelEditorView;
using tdme::tools::leveleditor::views::ParticleSystemView;
using tdme::tools::leveleditor::views::TriggerView;
using tdme::tools::shared::controller::FileDialogScreenController;
using tdme::tools::shared::controller::InfoDialogScreenController;
using tdme::tools::shared::model::LevelEditorEntity_EntityType;
using tdme::tools::shared::model::LevelEditorEntity;
using tdme::tools::shared::model::LevelEditorEntityLibrary;
using tdme::tools::shared::model::LevelEditorLevel;
using tdme::tools::shared::tools::Tools;
using tdme::tools::shared::views::PopUps;
using tdme::tools::shared::views::View;
using tdme::utils::MutableString;
using tdme::utils::Console;
using tdme::utils::Exception;

LevelEditorEntityLibraryScreenController::LevelEditorEntityLibraryScreenController(PopUps* popUps) 
{
	this->popUps = popUps;
	this->modelPath = ".";
}

GUIScreenNode* LevelEditorEntityLibraryScreenController::getScreenNode()
{
	return screenNode;
}

const string& LevelEditorEntityLibraryScreenController::getModelPath()
{
	return modelPath;
}

void LevelEditorEntityLibraryScreenController::setModelPath(const string& modelPath)
{
	this->modelPath = modelPath;
}

void LevelEditorEntityLibraryScreenController::initialize()
{
	try {
		screenNode = GUIParser::parse("resources/tools/leveleditor/gui", "screen_leveleditor_entitylibrary.xml");
		screenNode->addActionListener(this);
		screenNode->addChangeListener(this);
		entityLibraryListBox = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("entity_library_listbox"));
		buttonEntityPlace = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("button_entity_place"));
		buttonLevelEdit = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("button_level_edit"));
	} catch (Exception& exception) {
		Console::print(string("LevelEditorEntityLibraryScreenController::initialize(): An error occurred: "));
		Console::println(string(exception.what()));
	}
	buttonEntityPlace->getController()->setDisabled(false);
	buttonLevelEdit->getController()->setDisabled(true);
}

void LevelEditorEntityLibraryScreenController::dispose()
{
}

void LevelEditorEntityLibraryScreenController::selectEntity(int32_t entityId)
{
	MutableString entityLibraryListBoxSelection;
	entityLibraryListBoxSelection.set(entityId);
	entityLibraryListBox->getController()->setValue(entityLibraryListBoxSelection);
}

void LevelEditorEntityLibraryScreenController::setEntityLibrary()
{
	MutableString entityLibraryListBoxSelection;
	auto entityLibrary = TDMELevelEditor::getInstance()->getEntityLibrary();
	entityLibraryListBoxSelection.set(entityLibraryListBox->getController()->getValue());
	auto entityLibraryListBoxInnerNode = dynamic_cast< GUIParentNode* >((entityLibraryListBox->getScreenNode()->getNodeById(entityLibraryListBox->getId() + "_inner")));
	auto idx = 1;
	string entityLibraryListBoxSubNodesXML;
	entityLibraryListBoxSubNodesXML =
		entityLibraryListBoxSubNodesXML +
		"<scrollarea-vertical id=\"" +
		entityLibraryListBox->getId() +
		"_inner_scrollarea\" width=\"100%\" height=\"100%\">\n";
	for (auto i = 0; i < entityLibrary->getEntityCount(); i++) {
		auto objectId = entityLibrary->getEntityAt(i)->getId();
		auto objectName = entityLibrary->getEntityAt(i)->getName();
		entityLibraryListBoxSubNodesXML =
			entityLibraryListBoxSubNodesXML +
			"<selectbox-option text=\"" +
			GUIParser::escapeQuotes(objectName) +
			"\" value=\"" +
			to_string(objectId) +
			"\" " +
			(i == 0 ? "selected=\"true\" " : "") +
			"/>\n";
	}
	entityLibraryListBoxSubNodesXML = entityLibraryListBoxSubNodesXML + "</scrollarea-vertical>\n";
	try {
		entityLibraryListBoxInnerNode->replaceSubNodes(entityLibraryListBoxSubNodesXML, false);
	} catch (Exception& exception) {
		Console::print(string("LevelEditorEntityLibraryScreenController::setEntityLibrary(): An error occurred: "));
		Console::println(string(exception.what()));
	}
	if (entityLibraryListBoxSelection.length() > 0) {
		entityLibraryListBox->getController()->setValue(entityLibraryListBoxSelection);
	}
	onEntitySelectionChanged();
	buttonEntityPlace->getController()->setDisabled(entityLibrary->getEntityCount() == 0);
}

void LevelEditorEntityLibraryScreenController::onEntitySelectionChanged()
{
	auto view = TDMELevelEditor::getInstance()->getView();
	if (dynamic_cast< LevelEditorView* >(view) != nullptr) {
		auto entity = TDMELevelEditor::getInstance()->getEntityLibrary()->getEntity(Tools::convertToIntSilent(entityLibraryListBox->getController()->getValue().getString()));
		if (entity != nullptr) {
			(dynamic_cast< LevelEditorView* >(view))->loadEntityFromLibrary(entity->getId());
		}
	}
}

void LevelEditorEntityLibraryScreenController::onEditEntity()
{
	auto entity = TDMELevelEditor::getInstance()->getEntityLibrary()->getEntity(Tools::convertToIntSilent(entityLibraryListBox->getController()->getValue().getString()));
	if (entity == nullptr) return;
	{
		auto v = entity->getType();
		if (v == LevelEditorEntity_EntityType::MODEL) {
			if (dynamic_cast< ModelEditorView* >(TDMELevelEditor::getInstance()->getView()) != nullptr == false) {
				TDMELevelEditor::getInstance()->switchToModelEditor();
			}
			(dynamic_cast< ModelEditorView* >(TDMELevelEditor::getInstance()->getView()))->setEntity(entity);
		} else
		if (v == LevelEditorEntity_EntityType::TRIGGER) {
			if (dynamic_cast< TriggerView* >(TDMELevelEditor::getInstance()->getView()) != nullptr == false) {
				TDMELevelEditor::getInstance()->switchToTriggerView();
			}
			(dynamic_cast< TriggerView* >(TDMELevelEditor::getInstance()->getView()))->setEntity(entity);
		} else
		if (v == LevelEditorEntity_EntityType::EMPTY) {
			if (dynamic_cast< EmptyView* >(TDMELevelEditor::getInstance()->getView()) != nullptr == false) {
				TDMELevelEditor::getInstance()->switchToEmptyView();
			}
			(dynamic_cast< EmptyView* >(TDMELevelEditor::getInstance()->getView()))->setEntity(entity);
		} else
		if (v == LevelEditorEntity_EntityType::PARTICLESYSTEM) {
			if (dynamic_cast< ParticleSystemView* >(TDMELevelEditor::getInstance()->getView()) != nullptr == false) {
				TDMELevelEditor::getInstance()->switchToParticleSystemView();
			}
			(dynamic_cast< ParticleSystemView* >(TDMELevelEditor::getInstance()->getView()))->setEntity(entity);
		}
	}

	buttonEntityPlace->getController()->setDisabled(true);
	buttonLevelEdit->getController()->setDisabled(false);
}

void LevelEditorEntityLibraryScreenController::onEditLevel()
{
	TDMELevelEditor::getInstance()->switchToLevelEditor();
	buttonEntityPlace->getController()->setDisabled(false);
	buttonLevelEdit->getController()->setDisabled(true);
}

void LevelEditorEntityLibraryScreenController::onPlaceEntity()
{
	auto entity = TDMELevelEditor::getInstance()->getEntityLibrary()->getEntity(Tools::convertToIntSilent(entityLibraryListBox->getController()->getValue().getString()));
	if (entity == nullptr) return;
	auto view = TDMELevelEditor::getInstance()->getView();
	if (dynamic_cast< LevelEditorView* >(view) != nullptr) {
		(dynamic_cast< LevelEditorView* >(view))->placeObject();
	}
}

void LevelEditorEntityLibraryScreenController::onDeleteEntity()
{
	auto entity = TDMELevelEditor::getInstance()->getEntityLibrary()->getEntity(Tools::convertToIntSilent(entityLibraryListBox->getController()->getValue().getString()));
	if (entity == nullptr) return;
	TDMELevelEditor::getInstance()->getLevel()->removeObjectsByEntityId(entity->getId());
	auto view = TDMELevelEditor::getInstance()->getView();
	if (dynamic_cast< LevelEditorView* >(view) != nullptr) {
		(dynamic_cast< LevelEditorView* >(view))->loadLevel();
	} else {
		TDMELevelEditor::getInstance()->switchToLevelEditor();
	}
	TDMELevelEditor::getInstance()->getLevel()->getEntityLibrary()->removeEntity(entity->getId());
	setEntityLibrary();
}

void LevelEditorEntityLibraryScreenController::onPartitionEntity()
{
	auto entity = TDMELevelEditor::getInstance()->getEntityLibrary()->getEntity(Tools::convertToIntSilent(entityLibraryListBox->getController()->getValue().getString()));
	if (entity == nullptr) return;
	TDMELevelEditor::getInstance()->getLevel()->removeObjectsByEntityId(entity->getId());
	auto view = TDMELevelEditor::getInstance()->getView();
	if (dynamic_cast< LevelEditorView* >(view) != nullptr) {
		(dynamic_cast< LevelEditorView* >(view))->loadLevel();
	} else {
		TDMELevelEditor::getInstance()->switchToLevelEditor();
	}
	TDMELevelEditor::getInstance()->getLevel()->getEntityLibrary()->removeEntity(entity->getId());
	setEntityLibrary();
}

void LevelEditorEntityLibraryScreenController::onValueChanged(GUIElementNode* node)
{
	if (node->getId().compare("entity_library_listbox") == 0) {
		onEntitySelectionChanged();
	} else
	if (node->getId().compare("dropdown_entity_action") == 0) {
		if (node->getController()->getValue().getString() == "edit") {
			onEditEntity();
		} else
		if (node->getController()->getValue().getString() == "delete") {
			onDeleteEntity();
		} else
		if (node->getController()->getValue().getString() == "partition") {
			onPartitionEntity();
		} else
		if (node->getController()->getValue().getString() == "create_model") {
			auto const entityLibrary = TDMELevelEditor::getInstance()->getEntityLibrary();
			vector<string> extensions = ModelReader::getModelExtensions();
			extensions.push_back("tmm");
			popUps->getFileDialogScreenController()->show(
				modelPath,
				"Load from: ",
				&extensions,
				"",
				new LevelEditorEntityLibraryScreenController_onValueChanged_1(this, entityLibrary)
			);
		} else
		if (node->getController()->getValue().getString() == "create_trigger") {
			try {
				auto model = TDMELevelEditor::getInstance()->getEntityLibrary()->addTrigger(LevelEditorEntityLibrary::ID_ALLOCATE, "New trigger", "", 1.0f, 1.0f, 1.0f);
				setEntityLibrary();
				entityLibraryListBox->getController()->setValue(MutableString(model->getId()));
				onEditEntity();
			} catch (Exception& exception) {
				popUps->getInfoDialogScreenController()->show(
					"Error",
					"An error occurred: " + (string(exception.what()))
				);
			}
		} else
		if (node->getController()->getValue().getString() == "create_empty") {
			try {
				auto model = TDMELevelEditor::getInstance()->getEntityLibrary()->addEmpty(LevelEditorEntityLibrary::ID_ALLOCATE, "New empty", "");
				setEntityLibrary();
				entityLibraryListBox->getController()->setValue(MutableString(model->getId()));
				onEditEntity();
			} catch (Exception& exception) {
				popUps->getInfoDialogScreenController()->show(
					"Error",
					"An error occurred: " + (string(exception.what()))
				 );
			}
		} else
		if (node->getController()->getValue().getString() == "create_light") {
		} else
		if (node->getController()->getValue().getString() == "create_particlesystem") {
			try {
				auto model = TDMELevelEditor::getInstance()->getEntityLibrary()->addParticleSystem(LevelEditorEntityLibrary::ID_ALLOCATE, "New particle system", "");
				setEntityLibrary();
				entityLibraryListBox->getController()->setValue(MutableString(model->getId()));
				onEditEntity();
			} catch (Exception& exception) {
				popUps->getInfoDialogScreenController()->show(
					"Error",
					"An error occurred: " + (string(exception.what()))
				 );
			}
		} else {
			Console::println("LevelEditorEntityLibraryScreenController::onValueChanged: dropdown_model_create: " + node->getController()->getValue().getString());
		}
		node->getController()->setValue(MutableString("action"));
	} else {
		Console::println("LevelEditorEntityLibraryScreenController::onValueChanged: " + node->getId());
	}
}

void LevelEditorEntityLibraryScreenController::onActionPerformed(GUIActionListener_Type* type, GUIElementNode* node)
{
	if (type == GUIActionListener_Type::PERFORMED) {
		if (node->getId().compare("button_entity_place") == 0) {
			onPlaceEntity();
		} else if (node->getId().compare("button_level_edit") == 0) {
			onEditLevel();
		} else {
			Console::println("LevelEditorScreenController::onActionPerformed: " + node->getId());
		}
	}
}
