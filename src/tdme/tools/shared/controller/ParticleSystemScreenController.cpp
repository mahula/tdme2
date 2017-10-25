#include <tdme/tools/shared/controller/ParticleSystemScreenController.h>

#include <string>
#include <vector>

#include <tdme/engine/Rotation.h>
#include <tdme/engine/Rotations.h>
#include <tdme/engine/Transformations.h>
#include <tdme/engine/model/Color4.h>
#include <tdme/engine/model/Color4Base.h>
#include <tdme/engine/primitives/OrientedBoundingBox.h>
#include <tdme/gui/GUIParser.h>
#include <tdme/gui/events/GUIActionListener_Type.h>
#include <tdme/gui/nodes/GUIElementNode.h>
#include <tdme/gui/nodes/GUINode.h>
#include <tdme/gui/nodes/GUINodeConditions.h>
#include <tdme/gui/nodes/GUINodeController.h>
#include <tdme/gui/nodes/GUIParentNode.h>
#include <tdme/gui/nodes/GUIScreenNode.h>
#include <tdme/gui/nodes/GUITextNode.h>
#include <tdme/math/Matrix4x4.h>
#include <tdme/math/Vector3.h>
#include <tdme/tools/shared/controller/EntityBaseSubScreenController.h>
#include <tdme/tools/shared/controller/EntityBoundingVolumeSubScreenController.h>
#include <tdme/tools/shared/controller/EntityDisplaySubScreenController.h>
#include <tdme/tools/shared/controller/FileDialogPath.h>
#include <tdme/tools/shared/controller/FileDialogScreenController.h>
#include <tdme/tools/shared/controller/InfoDialogScreenController.h>
#include <tdme/tools/shared/controller/ParticleSystemScreenController_onParticleSystemLoad_2.h>
#include <tdme/tools/shared/controller/ParticleSystemScreenController_onEntitySave_3.h>
#include <tdme/tools/shared/controller/ParticleSystemScreenController_onActionPerformed_4.h>
#include <tdme/tools/shared/controller/ParticleSystemScreenController_ParticleSystemScreenController_1.h>
#include <tdme/tools/shared/model/LevelEditorEntity.h>
#include <tdme/tools/shared/model/LevelEditorEntityParticleSystem_BoundingBoxParticleEmitter.h>
#include <tdme/tools/shared/model/LevelEditorEntityParticleSystem_CircleParticleEmitter.h>
#include <tdme/tools/shared/model/LevelEditorEntityParticleSystem_CircleParticleEmitterPlaneVelocity.h>
#include <tdme/tools/shared/model/LevelEditorEntityParticleSystem_Emitter.h>
#include <tdme/tools/shared/model/LevelEditorEntityParticleSystem_ObjectParticleSystem.h>
#include <tdme/tools/shared/model/LevelEditorEntityParticleSystem_PointParticleEmitter.h>
#include <tdme/tools/shared/model/LevelEditorEntityParticleSystem_PointParticleSystem.h>
#include <tdme/tools/shared/model/LevelEditorEntityParticleSystem_SphereParticleEmitter.h>
#include <tdme/tools/shared/model/LevelEditorEntityParticleSystem_Type.h>
#include <tdme/tools/shared/model/LevelEditorEntityParticleSystem.h>
#include <tdme/tools/shared/tools/Tools.h>
#include <tdme/tools/shared/views/PopUps.h>
#include <tdme/tools/shared/views/SharedParticleSystemView.h>
#include <tdme/tools/viewer/TDMEViewer.h>
#include <tdme/utils/MutableString.h>
#include <tdme/utils/Console.h>
#include <tdme/utils/Exception.h>

using std::vector;
using std::string;

using tdme::tools::shared::controller::ParticleSystemScreenController;
using tdme::engine::Rotation;
using tdme::engine::Rotations;
using tdme::engine::Transformations;
using tdme::engine::model::Color4;
using tdme::engine::model::Color4Base;
using tdme::engine::primitives::OrientedBoundingBox;
using tdme::gui::GUIParser;
using tdme::gui::events::GUIActionListener_Type;
using tdme::gui::nodes::GUIElementNode;
using tdme::gui::nodes::GUINode;
using tdme::gui::nodes::GUINodeConditions;
using tdme::gui::nodes::GUINodeController;
using tdme::gui::nodes::GUIParentNode;
using tdme::gui::nodes::GUIScreenNode;
using tdme::gui::nodes::GUITextNode;
using tdme::math::Matrix4x4;
using tdme::math::Vector3;
using tdme::tools::shared::controller::EntityBaseSubScreenController;
using tdme::tools::shared::controller::EntityBoundingVolumeSubScreenController;
using tdme::tools::shared::controller::EntityDisplaySubScreenController;
using tdme::tools::shared::controller::FileDialogPath;
using tdme::tools::shared::controller::FileDialogScreenController;
using tdme::tools::shared::controller::InfoDialogScreenController;
using tdme::tools::shared::controller::ParticleSystemScreenController_onParticleSystemLoad_2;
using tdme::tools::shared::controller::ParticleSystemScreenController_onEntitySave_3;
using tdme::tools::shared::controller::ParticleSystemScreenController_onActionPerformed_4;
using tdme::tools::shared::controller::ParticleSystemScreenController_ParticleSystemScreenController_1;
using tdme::tools::shared::model::LevelEditorEntity;
using tdme::tools::shared::model::LevelEditorEntityParticleSystem_BoundingBoxParticleEmitter;
using tdme::tools::shared::model::LevelEditorEntityParticleSystem_CircleParticleEmitter;
using tdme::tools::shared::model::LevelEditorEntityParticleSystem_CircleParticleEmitterPlaneVelocity;
using tdme::tools::shared::model::LevelEditorEntityParticleSystem_Emitter;
using tdme::tools::shared::model::LevelEditorEntityParticleSystem_ObjectParticleSystem;
using tdme::tools::shared::model::LevelEditorEntityParticleSystem_PointParticleEmitter;
using tdme::tools::shared::model::LevelEditorEntityParticleSystem_PointParticleSystem;
using tdme::tools::shared::model::LevelEditorEntityParticleSystem_SphereParticleEmitter;
using tdme::tools::shared::model::LevelEditorEntityParticleSystem_Type;
using tdme::tools::shared::model::LevelEditorEntityParticleSystem;
using tdme::tools::shared::tools::Tools;
using tdme::tools::shared::views::PopUps;
using tdme::tools::shared::views::SharedParticleSystemView;
using tdme::tools::viewer::TDMEViewer;
using tdme::utils::MutableString;
using tdme::utils::Exception;
using tdme::utils::Console;

string ParticleSystemScreenController::TYPE_NONE = "None";
string ParticleSystemScreenController::TYPE_OBJECTPARTICLESYSTEM = "Object Particle System";
string ParticleSystemScreenController::TYPE_POINTSPARTICLESYSTEM = "Points Particle System";
string ParticleSystemScreenController::EMITTER_NONE = "None";
string ParticleSystemScreenController::EMITTER_POINTPARTICLEEMITTER = "Point Particle Emitter";
string ParticleSystemScreenController::EMITTER_BOUNDINGBOXPARTICLEEMITTER = "BoundingBox Particle Emitter";
string ParticleSystemScreenController::EMITTER_CIRCLEPARTICLEEMITTER = "Circle Particle Emitter";
string ParticleSystemScreenController::EMITTER_CIRCLEPARTICLEEMITTERPLANEVELOCITY = "Circle Particle Emitter Plane Velocity";
string ParticleSystemScreenController::EMITTER_SPHEREPARTICLEEMITTER = "Sphere Particle Emitter";

ParticleSystemScreenController::ParticleSystemScreenController(SharedParticleSystemView* view)
{
	this->particleSystemPath = new FileDialogPath(".");
	this->modelPath = new FileDialogPath(".");
	this->view = view;
	auto const finalView = view;
	this->entityBaseSubScreenController = new EntityBaseSubScreenController(view->getPopUpsViews(), new ParticleSystemScreenController_ParticleSystemScreenController_1(this, finalView));
	this->entityDisplaySubScreenController = new EntityDisplaySubScreenController();
	this->entityBoundingVolumeSubScreenController = new EntityBoundingVolumeSubScreenController(view->getPopUpsViews(), particleSystemPath);
	this->value = new MutableString();
}

ParticleSystemScreenController::~ParticleSystemScreenController() {
	delete particleSystemPath;
	delete modelPath;
	delete entityBaseSubScreenController;
	delete entityDisplaySubScreenController;
	delete entityBoundingVolumeSubScreenController;
}

EntityDisplaySubScreenController* ParticleSystemScreenController::getEntityDisplaySubScreenController()
{
	return entityDisplaySubScreenController;
}

EntityBoundingVolumeSubScreenController* ParticleSystemScreenController::getEntityBoundingVolumeSubScreenController()
{
	return entityBoundingVolumeSubScreenController;
}

GUIScreenNode* ParticleSystemScreenController::getScreenNode()
{
	return screenNode;
}

FileDialogPath* ParticleSystemScreenController::getParticleSystemPath()
{
	return particleSystemPath;
}

FileDialogPath* ParticleSystemScreenController::getModelPath()
{
	return modelPath;
}

void ParticleSystemScreenController::initialize()
{
	try {
		screenNode = GUIParser::parse("resources/tools/particlesystem/gui", "screen_particlesystem.xml");
		screenNode->addActionListener(this);
		screenNode->addChangeListener(this);
		screenCaption = dynamic_cast< GUITextNode* >(screenNode->getNodeById("screen_caption"));
		particleSystemReload = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("button_entity_reload"));
		particleSystemSave = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("button_entity_save"));
		particleSystemTypes = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("ps_types"));
		particleSystemType = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("ps_type"));
		particleSystemEmitters = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("ps_emitters"));
		particleSystemEmitter = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("ps_emitter"));
		opsScale = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("ops_scale"));
		opsMaxCount = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("ops_maxcount"));
		opsModel = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("ops_model"));
		opsAutoEmit = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("ops_auto_emit"));
		ppsMaxPoints = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("pps_maxpoints"));
		ppsAutoEmit = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("pps_auto_emit"));
		ppeCount = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("ppe_count"));
		ppeLifeTime = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("ppe_lifetime"));
		ppeLifeTimeRnd = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("ppe_lifetimernd"));
		ppeMass = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("ppe_mass"));
		ppeMassRnd = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("ppe_massrnd"));
		ppePosition = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("ppe_position"));
		ppeVelocity = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("ppe_velocity"));
		ppeVelocityRnd = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("ppe_velocityrnd"));
		ppeColorStart = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("ppe_colorstart"));
		ppeColorEnd = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("ppe_colorend"));
		bbpeCount = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("bbpe_count"));
		bbpeLifeTime = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("bbpe_lifetime"));
		bbpeLifeTimeRnd = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("bbpe_lifetimernd"));
		bbpeMass = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("bbpe_mass"));
		bbpeMassRnd = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("bbpe_massrnd"));
		bbpeVelocity = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("bbpe_velocity"));
		bbpeVelocityRnd = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("bbpe_velocityrnd"));
		bbpeColorStart = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("bbpe_colorstart"));
		bbpeColorEnd = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("bbpe_colorend"));
		bbpeObbCenter = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("bbpe_obb_center"));
		bbpeObbHalfextension = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("bbpe_obb_halfextension"));
		bbpeObbRotationX = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("bbpe_obb_rotation_x"));
		bbpeObbRotationY = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("bbpe_obb_rotation_y"));
		bbpeObbRotationZ = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("bbpe_obb_rotation_z"));
		cpeCount = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("cpe_count"));
		cpeLifeTime = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("cpe_lifetime"));
		cpeLifeTimeRnd = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("cpe_lifetimernd"));
		cpeMass = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("cpe_mass"));
		cpeMassRnd = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("cpe_massrnd"));
		cpeVelocity = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("cpe_velocity"));
		cpeVelocityRnd = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("cpe_velocityrnd"));
		cpeColorStart = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("cpe_colorstart"));
		cpeColorEnd = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("cpe_colorend"));
		cpeCenter = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("cpe_center"));
		cpeRadius = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("cpe_radius"));
		cpeRotationX = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("cpe_rotation_x"));
		cpeRotationY = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("cpe_rotation_y"));
		cpeRotationZ = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("cpe_rotation_z"));
		cpepvCount = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("cpepv_count"));
		cpepvLifeTime = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("cpepv_lifetime"));
		cpepvLifeTimeRnd = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("cpepv_lifetimernd"));
		cpepvMass = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("cpepv_mass"));
		cpepvMassRnd = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("cpepv_massrnd"));
		cpepvVelocity = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("cpepv_velocity"));
		cpepvVelocityRnd = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("cpepv_velocityrnd"));
		cpepvColorStart = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("cpepv_colorstart"));
		cpepvColorEnd = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("cpepv_colorend"));
		cpepvCenter = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("cpepv_center"));
		cpepvRadius = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("cpepv_radius"));
		cpepvRotationX = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("cpepv_rotation_x"));
		cpepvRotationY = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("cpepv_rotation_y"));
		cpepvRotationZ = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("cpepv_rotation_z"));
		speCount = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("spe_count"));
		speLifeTime = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("spe_lifetime"));
		speLifeTimeRnd = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("spe_lifetimernd"));
		speMass = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("spe_mass"));
		speMassRnd = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("spe_massrnd"));
		speVelocity = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("spe_velocity"));
		speVelocityRnd = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("spe_velocityrnd"));
		speColorStart = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("spe_colorstart"));
		speColorEnd = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("spe_colorend"));
		speCenter = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("spe_center"));
		speRadius = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("spe_radius"));
	} catch (Exception& exception) {
		Console::print(string("ParticleSystemScreenController::initialize(): An error occurred: "));
		Console::println(string(exception.what()));
	}
	entityBaseSubScreenController->initialize(screenNode);
	entityDisplaySubScreenController->initialize(screenNode);
	entityBoundingVolumeSubScreenController->initialize(screenNode);
}

void ParticleSystemScreenController::dispose()
{
}

void ParticleSystemScreenController::setScreenCaption(const string& text)
{
	screenCaption->getText()->set(text);
	screenNode->layout(screenCaption);
}

void ParticleSystemScreenController::setEntityData(const string& name, const string& description)
{
	entityBaseSubScreenController->setEntityData(name, description);
	particleSystemReload->getController()->setDisabled(false);
	particleSystemSave->getController()->setDisabled(false);
}

void ParticleSystemScreenController::unsetEntityData()
{
	entityBaseSubScreenController->unsetEntityData();
	particleSystemReload->getController()->setDisabled(true);
	particleSystemSave->getController()->setDisabled(true);
}

void ParticleSystemScreenController::setEntityProperties(const string& presetId, LevelEditorEntity* entity, const string& selectedName)
{
	entityBaseSubScreenController->setEntityProperties(view->getEntity(), presetId, selectedName);
}

void ParticleSystemScreenController::unsetEntityProperties()
{
	entityBaseSubScreenController->unsetEntityProperties();
}

void ParticleSystemScreenController::setParticleSystemTypes(const vector<string>* particleSystemTypesCollection)
{
	auto particleSystemTypesInnerNode = dynamic_cast< GUIParentNode* >(particleSystemTypes->getScreenNode()->getNodeById(particleSystemTypes->getId() + "_inner"));
	auto idx = 0;
	string particleSystemTypesInnerNodeSubNodesXML = "";
	particleSystemTypesInnerNodeSubNodesXML =
		particleSystemTypesInnerNodeSubNodesXML +
		"<scrollarea-vertical id=\"" +
		particleSystemTypes->getId() +
		"_inner_scrollarea\" width=\"100%\" height=\"100\">\n";
	for (auto particleSystem: *particleSystemTypesCollection) {
		particleSystemTypesInnerNodeSubNodesXML =
			particleSystemTypesInnerNodeSubNodesXML +
			"<dropdown-option text=\"" +
			GUIParser::escapeQuotes(particleSystem) +
			"\" value=\"" +
			GUIParser::escapeQuotes(particleSystem) +
			"\" " +
			(idx == 0 ? "selected=\"true\" " : "") +
			" />\n";
		idx++;
	}
	particleSystemTypesInnerNodeSubNodesXML =
		particleSystemTypesInnerNodeSubNodesXML +
		"</scrollarea-vertical>";
	try {
		particleSystemTypesInnerNode->replaceSubNodes(particleSystemTypesInnerNodeSubNodesXML, true);
	} catch (Exception& exception) {
		Console::print(string("ParticleSystemScreenController::setParticleSystemTypes(): An error occurred: "));
		Console::println(string(exception.what()));
	}
}

void ParticleSystemScreenController::setParticleSystemEmitters(const vector<string>* emittersCollection)
{
	auto particleSystemEmittersInnerNode = dynamic_cast< GUIParentNode* >((particleSystemEmitters->getScreenNode()->getNodeById(particleSystemEmitters->getId() + "_inner")));
	auto idx = 0;
	string particleSystemEmittersInnerNodeSubNodesXML = "";
	particleSystemEmittersInnerNodeSubNodesXML =
		particleSystemEmittersInnerNodeSubNodesXML +
		"<scrollarea-vertical id=\"" +
		particleSystemEmitters->getId() +
		"_inner_scrollarea\" width=\"100%\" height=\"100\">\n";
	for (auto particleSystemEmitter: *emittersCollection) {
		particleSystemEmittersInnerNodeSubNodesXML =
			particleSystemEmittersInnerNodeSubNodesXML +
			"<dropdown-option text=\"" +
			GUIParser::escapeQuotes(particleSystemEmitter) +
			"\" value=\"" +
			GUIParser::escapeQuotes(particleSystemEmitter) +
			"\" " +
			(idx == 0 ? "selected=\"true\" " : "") +
			" />\n";
		idx++;
	}
	particleSystemEmittersInnerNodeSubNodesXML =
		particleSystemEmittersInnerNodeSubNodesXML +
		"</scrollarea-vertical>";
	try {
		particleSystemEmittersInnerNode->replaceSubNodes(particleSystemEmittersInnerNodeSubNodesXML, true);
	} catch (Exception& exception) {
		Console::print(string("ParticleSystemScreenController::setParticleSystemEmitters: An error occurred: "));
		Console::println(string(exception.what()));
	}
}

void ParticleSystemScreenController::onQuit()
{
	TDMEViewer::getInstance()->quit();
}

void ParticleSystemScreenController::setParticleSystemType()
{
	auto particleSystem = view->getEntity()->getParticleSystem();
	particleSystemType->getActiveConditions()->removeAll();
	{
		auto v = particleSystem->getType();
		if ((v == LevelEditorEntityParticleSystem_Type::NONE)) {
			particleSystemTypes->getController()->setValue(value->set(TYPE_NONE));
			particleSystemType->getActiveConditions()->add(TYPE_NONE);
			goto end_switch0;;
		}
		if ((v == LevelEditorEntityParticleSystem_Type::OBJECT_PARTICLE_SYSTEM)) {
			particleSystemTypes->getController()->setValue(value->set(TYPE_OBJECTPARTICLESYSTEM));
			particleSystemType->getActiveConditions()->add(TYPE_OBJECTPARTICLESYSTEM);
			opsMaxCount->getController()->setValue(value->set(particleSystem->getObjectParticleSystem()->getMaxCount()));
			opsScale->getController()->setValue(value->set(Tools::formatVector3(particleSystem->getObjectParticleSystem()->getScale())));
			opsModel->getController()->setValue(value->set(particleSystem->getObjectParticleSystem()->getModelFile()));
			opsAutoEmit->getController()->setValue(value->set(particleSystem->getObjectParticleSystem()->isAutoEmit() == true ? "1" : ""));
			goto end_switch0;;
		}
		if ((v == LevelEditorEntityParticleSystem_Type::POINT_PARTICLE_SYSTEM)) {
			particleSystemTypes->getController()->setValue(value->set(TYPE_POINTSPARTICLESYSTEM));
			particleSystemType->getActiveConditions()->add(TYPE_POINTSPARTICLESYSTEM);
			ppsMaxPoints->getController()->setValue(value->set(particleSystem->getPointParticleSystem()->getMaxPoints()));
			ppsAutoEmit->getController()->setValue(value->set(particleSystem->getPointParticleSystem()->isAutoEmit() == true ? "1" : ""));
			goto end_switch0;;
		}
		if ((((v != LevelEditorEntityParticleSystem_Type::NONE) && (v != LevelEditorEntityParticleSystem_Type::OBJECT_PARTICLE_SYSTEM) && (v != LevelEditorEntityParticleSystem_Type::POINT_PARTICLE_SYSTEM)))) {
			Console::println(
				string(
					"ParticleSystemScreenController::setParticleSystemType(): unknown particle system type '" +
					particleSystem->getType()->getName() +
					"'"
				)
			);
			goto end_switch0;
		}
		end_switch0:;
	}

	view->initParticleSystemRequest();
}

void ParticleSystemScreenController::onParticleSystemTypeDataApply()
{
	try {
		auto particleSystem = view->getEntity()->getParticleSystem();
		{
			auto v = particleSystem->getType();
			if ((v == LevelEditorEntityParticleSystem_Type::NONE)) {
				goto end_switch1;;
			}
			if ((v == LevelEditorEntityParticleSystem_Type::OBJECT_PARTICLE_SYSTEM)) {
				particleSystem->getObjectParticleSystem()->setMaxCount(Tools::convertToInt(opsMaxCount->getController()->getValue()->toWString()));
				particleSystem->getObjectParticleSystem()->getScale().set(Tools::convertToVector3(opsScale->getController()->getValue()->toWString()));
				particleSystem->getObjectParticleSystem()->setAutoEmit(opsAutoEmit->getController()->getValue()->toWString() == "1");
				try {
					particleSystem->getObjectParticleSystem()->setModelFile(opsModel->getController()->getValue()->toWString());
				} catch (Exception& exception) {
					view->getPopUpsViews()->getInfoDialogScreenController()->show("Error", "An error occurred: " + (string(exception.what())));
				}
				goto end_switch1;;
			}
			if ((v == LevelEditorEntityParticleSystem_Type::POINT_PARTICLE_SYSTEM)) {
				particleSystem->getPointParticleSystem()->setMaxPoints(Tools::convertToInt(ppsMaxPoints->getController()->getValue()->toWString()));
				particleSystem->getPointParticleSystem()->setAutoEmit(ppsAutoEmit->getController()->getValue()->toWString() == "1");
				goto end_switch1;;
			}
			if ((((v != LevelEditorEntityParticleSystem_Type::NONE) && (v != LevelEditorEntityParticleSystem_Type::OBJECT_PARTICLE_SYSTEM) && (v != LevelEditorEntityParticleSystem_Type::POINT_PARTICLE_SYSTEM)))) {
				Console::println(
					string(
						"ParticleSystemScreenController::setParticleSystemType(): unknown particle system type '" +
						particleSystem->getType()->getName() +
						"'"
					)
				);
				goto end_switch1;;
			}
			end_switch1:;
		}

	} catch (Exception& exception) {
		showErrorPopUp("Warning", (string(exception.what())));
	}
	view->initParticleSystemRequest();
}

void ParticleSystemScreenController::onParticleSystemTypeApply()
{
	auto particleSystemTypeString = particleSystemTypes->getController()->getValue()->toWString();
	particleSystemType->getActiveConditions()->removeAll();
	particleSystemType->getActiveConditions()->add(particleSystemTypeString);
	if (particleSystemTypeString == TYPE_NONE) {
		view->getEntity()->getParticleSystem()->setType(LevelEditorEntityParticleSystem_Type::NONE);
	} else
	if (particleSystemTypeString == TYPE_OBJECTPARTICLESYSTEM) {
		view->getEntity()->getParticleSystem()->setType(LevelEditorEntityParticleSystem_Type::OBJECT_PARTICLE_SYSTEM);
	} else
	if (particleSystemTypeString == TYPE_POINTSPARTICLESYSTEM) {
		view->getEntity()->getParticleSystem()->setType(LevelEditorEntityParticleSystem_Type::POINT_PARTICLE_SYSTEM);
	} else {
		Console::println(
			string(
				"ParticleSystemScreenController::onParticleSystemTypeApply(): unknown particle system type '" +
				particleSystemTypeString +
				"'"
			)
		);
	}
	setParticleSystemType();
	view->initParticleSystemRequest();
}

void ParticleSystemScreenController::onParticleSystemEmitterApply()
{
	auto particleSystem = view->getEntity()->getParticleSystem();
	auto particleSystemEmitterString = particleSystemEmitters->getController()->getValue()->toWString();
	particleSystemEmitter->getActiveConditions()->removeAll();
	particleSystemEmitter->getActiveConditions()->add(particleSystemEmitterString);
	if (particleSystemEmitterString == EMITTER_NONE) {
		particleSystem->setEmitter(LevelEditorEntityParticleSystem_Emitter::NONE);
	} else
	if (particleSystemEmitterString == EMITTER_POINTPARTICLEEMITTER) {
		particleSystem->setEmitter(LevelEditorEntityParticleSystem_Emitter::POINT_PARTICLE_EMITTER);
	} else
	if (particleSystemEmitterString == EMITTER_BOUNDINGBOXPARTICLEEMITTER) {
		particleSystem->setEmitter(LevelEditorEntityParticleSystem_Emitter::BOUNDINGBOX_PARTICLE_EMITTER);
	} else
	if (particleSystemEmitterString == EMITTER_CIRCLEPARTICLEEMITTER) {
		particleSystem->setEmitter(LevelEditorEntityParticleSystem_Emitter::CIRCLE_PARTICLE_EMITTER);
	} else
	if (particleSystemEmitterString == EMITTER_CIRCLEPARTICLEEMITTERPLANEVELOCITY) {
		particleSystem->setEmitter(LevelEditorEntityParticleSystem_Emitter::CIRCLE_PARTICLE_EMITTER_PLANE_VELOCITY);
	} else
	if (particleSystemEmitterString == EMITTER_SPHEREPARTICLEEMITTER) {
		particleSystem->setEmitter(LevelEditorEntityParticleSystem_Emitter::SPHERE_PARTICLE_EMITTER);
	} else {
		Console::println(
			string(
				"ParticleSystemScreenController::onParticleSystemEmitterApply(): unknown particle system emitter '" +
				particleSystemEmitterString +
				"'"
			)
		);
	}
	setParticleSystemEmitter();
}

void ParticleSystemScreenController::onParticleSystemEmitterDataApply()
{
	try {
		auto particleSystem = view->getEntity()->getParticleSystem();
		{
			auto v = particleSystem->getEmitter();
			if ((v == LevelEditorEntityParticleSystem_Emitter::NONE))
			{
				{
					particleSystemEmitters->getController()->setValue(value->set(EMITTER_NONE));
					goto end_switch2;;
				}
			}
			if ((v == LevelEditorEntityParticleSystem_Emitter::NONE) || (v == LevelEditorEntityParticleSystem_Emitter::POINT_PARTICLE_EMITTER))
			{
				{
					particleSystemEmitters->getController()->setValue(value->set(EMITTER_POINTPARTICLEEMITTER));
					auto emitter = particleSystem->getPointParticleEmitter();
					emitter->setCount(Tools::convertToInt(ppeCount->getController()->getValue()->toWString()));
					emitter->setLifeTime(Tools::convertToInt(ppeLifeTime->getController()->getValue()->toWString()));
					emitter->setLifeTimeRnd(Tools::convertToInt(ppeLifeTimeRnd->getController()->getValue()->toWString()));
					emitter->setMass(Tools::convertToFloat(ppeMass->getController()->getValue()->toWString()));
					emitter->setMassRnd(Tools::convertToFloat(ppeMassRnd->getController()->getValue()->toWString()));
					emitter->getPosition().set(Tools::convertToVector3(ppePosition->getController()->getValue()->toWString()));
					emitter->getVelocity().set(Tools::convertToVector3(ppeVelocity->getController()->getValue()->toWString()));
					emitter->getVelocityRnd().set(Tools::convertToVector3(ppeVelocityRnd->getController()->getValue()->toWString()));
					emitter->getColorStart().set(Tools::convertToColor4(ppeColorStart->getController()->getValue()->toWString()));
					emitter->getColorEnd().set(Tools::convertToColor4(ppeColorEnd->getController()->getValue()->toWString()));
					goto end_switch2;;
				}
			}
			if ((v == LevelEditorEntityParticleSystem_Emitter::NONE) || (v == LevelEditorEntityParticleSystem_Emitter::POINT_PARTICLE_EMITTER) || (v == LevelEditorEntityParticleSystem_Emitter::BOUNDINGBOX_PARTICLE_EMITTER))
			{
				{
					particleSystemEmitters->getController()->setValue(value->set(EMITTER_BOUNDINGBOXPARTICLEEMITTER));
					auto emitter = particleSystem->getBoundingBoxParticleEmitters();
					emitter->setCount(Tools::convertToInt(bbpeCount->getController()->getValue()->toWString()));
					emitter->setLifeTime(Tools::convertToInt(bbpeLifeTime->getController()->getValue()->toWString()));
					emitter->setLifeTimeRnd(Tools::convertToInt(bbpeLifeTimeRnd->getController()->getValue()->toWString()));
					emitter->setMass(Tools::convertToFloat(bbpeMass->getController()->getValue()->toWString()));
					emitter->setMassRnd(Tools::convertToFloat(bbpeMassRnd->getController()->getValue()->toWString()));
					emitter->getVelocity().set(Tools::convertToVector3(bbpeVelocity->getController()->getValue()->toWString()));
					emitter->getVelocityRnd().set(Tools::convertToVector3(bbpeVelocityRnd->getController()->getValue()->toWString()));
					emitter->getColorStart().set(Tools::convertToColor4(bbpeColorStart->getController()->getValue()->toWString()));
					emitter->getColorEnd().set(Tools::convertToColor4(bbpeColorEnd->getController()->getValue()->toWString()));
					emitter->getObbCenter().set(Tools::convertToVector3(bbpeObbCenter->getController()->getValue()->toWString()));
					emitter->getObbHalfextension().set(Tools::convertToVector3(bbpeObbHalfextension->getController()->getValue()->toWString()));
					Transformations transformations;
					transformations.getRotations()->add(new Rotation(Tools::convertToFloat(bbpeObbRotationZ->getController()->getValue()->toWString()), OrientedBoundingBox::AABB_AXIS_Z));
					transformations.getRotations()->add(new Rotation(Tools::convertToFloat(bbpeObbRotationY->getController()->getValue()->toWString()), OrientedBoundingBox::AABB_AXIS_Y));
					transformations.getRotations()->add(new Rotation(Tools::convertToFloat(bbpeObbRotationX->getController()->getValue()->toWString()), OrientedBoundingBox::AABB_AXIS_X));
					transformations.update();
					transformations.getTransformationsMatrix().getAxes(emitter->getObbAxis0(), emitter->getObbAxis1(), emitter->getObbAxis2());
					goto end_switch2;;
				}
			}
			if ((v == LevelEditorEntityParticleSystem_Emitter::NONE) || (v == LevelEditorEntityParticleSystem_Emitter::POINT_PARTICLE_EMITTER) || (v == LevelEditorEntityParticleSystem_Emitter::BOUNDINGBOX_PARTICLE_EMITTER) || (v == LevelEditorEntityParticleSystem_Emitter::CIRCLE_PARTICLE_EMITTER))
			{
				{
					particleSystemEmitters->getController()->setValue(value->set(EMITTER_CIRCLEPARTICLEEMITTER));
					auto emitter = particleSystem->getCircleParticleEmitter();
					emitter->setCount(Tools::convertToInt(cpeCount->getController()->getValue()->toWString()));
					emitter->setLifeTime(Tools::convertToInt(cpeLifeTime->getController()->getValue()->toWString()));
					emitter->setLifeTimeRnd(Tools::convertToInt(cpeLifeTimeRnd->getController()->getValue()->toWString()));
					emitter->setMass(Tools::convertToFloat(cpeMass->getController()->getValue()->toWString()));
					emitter->setMassRnd(Tools::convertToFloat(cpeMassRnd->getController()->getValue()->toWString()));
					emitter->getVelocity().set(Tools::convertToVector3(cpeVelocity->getController()->getValue()->toWString()));
					emitter->getVelocityRnd().set(Tools::convertToVector3(cpeVelocityRnd->getController()->getValue()->toWString()));
					emitter->getColorStart().set(Tools::convertToColor4(cpeColorStart->getController()->getValue()->toWString()));
					emitter->getColorEnd().set(Tools::convertToColor4(cpeColorEnd->getController()->getValue()->toWString()));
					emitter->getCenter().set(Tools::convertToVector3(cpeCenter->getController()->getValue()->toWString()));
					emitter->setRadius(Tools::convertToFloat(cpeRadius->getController()->getValue()->toWString()));
					Transformations transformations;
					transformations.getRotations()->add(new Rotation(Tools::convertToFloat(cpeRotationZ->getController()->getValue()->toWString()), OrientedBoundingBox::AABB_AXIS_Z));
					transformations.getRotations()->add(new Rotation(Tools::convertToFloat(cpeRotationY->getController()->getValue()->toWString()), OrientedBoundingBox::AABB_AXIS_Y));
					transformations.getRotations()->add(new Rotation(Tools::convertToFloat(cpeRotationX->getController()->getValue()->toWString()), OrientedBoundingBox::AABB_AXIS_X));
					transformations.update();
					Vector3 tmpVector3;
					transformations.getTransformationsMatrix().getAxes(emitter->getAxis0(), tmpVector3, emitter->getAxis1());
					goto end_switch2;;
				}
			}
			if ((v == LevelEditorEntityParticleSystem_Emitter::NONE) || (v == LevelEditorEntityParticleSystem_Emitter::POINT_PARTICLE_EMITTER) || (v == LevelEditorEntityParticleSystem_Emitter::BOUNDINGBOX_PARTICLE_EMITTER) || (v == LevelEditorEntityParticleSystem_Emitter::CIRCLE_PARTICLE_EMITTER) || (v == LevelEditorEntityParticleSystem_Emitter::CIRCLE_PARTICLE_EMITTER_PLANE_VELOCITY))
			{
				{
					particleSystemEmitters->getController()->setValue(value->set(EMITTER_CIRCLEPARTICLEEMITTERPLANEVELOCITY));
					auto emitter = particleSystem->getCircleParticleEmitterPlaneVelocity();
					emitter->setCount(Tools::convertToInt(cpepvCount->getController()->getValue()->toWString()));
					emitter->setLifeTime(Tools::convertToInt(cpepvLifeTime->getController()->getValue()->toWString()));
					emitter->setLifeTimeRnd(Tools::convertToInt(cpepvLifeTimeRnd->getController()->getValue()->toWString()));
					emitter->setMass(Tools::convertToFloat(cpepvMass->getController()->getValue()->toWString()));
					emitter->setMassRnd(Tools::convertToFloat(cpepvMassRnd->getController()->getValue()->toWString()));
					emitter->setVelocity(Tools::convertToFloat(cpepvVelocity->getController()->getValue()->toWString()));
					emitter->setVelocityRnd(Tools::convertToFloat(cpepvVelocityRnd->getController()->getValue()->toWString()));
					emitter->getColorStart().set(Tools::convertToColor4(cpepvColorStart->getController()->getValue()->toWString()));
					emitter->getColorEnd().set(Tools::convertToColor4(cpepvColorEnd->getController()->getValue()->toWString()));
					emitter->getCenter().set(Tools::convertToVector3(cpepvCenter->getController()->getValue()->toWString()));
					emitter->setRadius(Tools::convertToFloat(cpepvRadius->getController()->getValue()->toWString()));
					Transformations transformations;
					transformations.getRotations()->add(new Rotation(Tools::convertToFloat(cpepvRotationZ->getController()->getValue()->toWString()), OrientedBoundingBox::AABB_AXIS_Z));
					transformations.getRotations()->add(new Rotation(Tools::convertToFloat(cpepvRotationY->getController()->getValue()->toWString()), OrientedBoundingBox::AABB_AXIS_Y));
					transformations.getRotations()->add(new Rotation(Tools::convertToFloat(cpepvRotationX->getController()->getValue()->toWString()), OrientedBoundingBox::AABB_AXIS_X));
					transformations.update();
					Vector3 tmpVector3;
					transformations.getTransformationsMatrix().getAxes(emitter->getAxis0(), tmpVector3, emitter->getAxis1());
					goto end_switch2;;
				}
			}
			if ((v == LevelEditorEntityParticleSystem_Emitter::NONE) || (v == LevelEditorEntityParticleSystem_Emitter::POINT_PARTICLE_EMITTER) || (v == LevelEditorEntityParticleSystem_Emitter::BOUNDINGBOX_PARTICLE_EMITTER) || (v == LevelEditorEntityParticleSystem_Emitter::CIRCLE_PARTICLE_EMITTER) || (v == LevelEditorEntityParticleSystem_Emitter::CIRCLE_PARTICLE_EMITTER_PLANE_VELOCITY) || (v == LevelEditorEntityParticleSystem_Emitter::SPHERE_PARTICLE_EMITTER))
			{
				{
					particleSystemEmitters->getController()->setValue(value->set(EMITTER_SPHEREPARTICLEEMITTER));
					auto emitter = particleSystem->getSphereParticleEmitter();
					emitter->setCount(Tools::convertToInt(speCount->getController()->getValue()->toWString()));
					emitter->setLifeTime(Tools::convertToInt(speLifeTime->getController()->getValue()->toWString()));
					emitter->setLifeTimeRnd(Tools::convertToInt(speLifeTimeRnd->getController()->getValue()->toWString()));
					emitter->setMass(Tools::convertToFloat(speMass->getController()->getValue()->toWString()));
					emitter->setMassRnd(Tools::convertToFloat(speMassRnd->getController()->getValue()->toWString()));
					emitter->getVelocity().set(Tools::convertToVector3(speVelocity->getController()->getValue()->toWString()));
					emitter->getVelocityRnd().set(Tools::convertToVector3(speVelocityRnd->getController()->getValue()->toWString()));
					emitter->getColorStart().set(Tools::convertToColor4(speColorStart->getController()->getValue()->toWString()));
					emitter->getColorEnd().set(Tools::convertToColor4(speColorEnd->getController()->getValue()->toWString()));
					emitter->getCenter().set(Tools::convertToVector3(speCenter->getController()->getValue()->toWString()));
					emitter->setRadius(Tools::convertToFloat(speRadius->getController()->getValue()->toWString()));
					goto end_switch2;;
				}
			}
			if (((v == LevelEditorEntityParticleSystem_Emitter::NONE) || (v == LevelEditorEntityParticleSystem_Emitter::POINT_PARTICLE_EMITTER) || (v == LevelEditorEntityParticleSystem_Emitter::BOUNDINGBOX_PARTICLE_EMITTER) || (v == LevelEditorEntityParticleSystem_Emitter::CIRCLE_PARTICLE_EMITTER) || (v == LevelEditorEntityParticleSystem_Emitter::CIRCLE_PARTICLE_EMITTER_PLANE_VELOCITY) || (v == LevelEditorEntityParticleSystem_Emitter::SPHERE_PARTICLE_EMITTER) || ((v != LevelEditorEntityParticleSystem_Emitter::NONE) && (v != LevelEditorEntityParticleSystem_Emitter::POINT_PARTICLE_EMITTER) && (v != LevelEditorEntityParticleSystem_Emitter::BOUNDINGBOX_PARTICLE_EMITTER) && (v != LevelEditorEntityParticleSystem_Emitter::CIRCLE_PARTICLE_EMITTER) && (v != LevelEditorEntityParticleSystem_Emitter::CIRCLE_PARTICLE_EMITTER_PLANE_VELOCITY) && (v != LevelEditorEntityParticleSystem_Emitter::SPHERE_PARTICLE_EMITTER))))
			{
				Console::println(
					string(
						"ParticleSystemScreenController::onParticleSystemEmitterApply(): unknown particle system emitter '" +
						particleSystem->getEmitter()->getName() +
						"'"
					)
				);
			}
			end_switch2:;
		}

	} catch (Exception& exception) {
		showErrorPopUp("Warning", (string(exception.what())));
	}
	view->initParticleSystemRequest();
}

void ParticleSystemScreenController::setParticleSystemEmitter()
{
	particleSystemEmitter->getActiveConditions()->removeAll();
	auto particleSystem = view->getEntity()->getParticleSystem();
	{
		auto v = particleSystem->getEmitter();
		if ((v == LevelEditorEntityParticleSystem_Emitter::NONE)) {
			{
				particleSystemEmitters->getController()->setValue(value->set(EMITTER_NONE));
				particleSystemEmitter->getActiveConditions()->add(EMITTER_NONE);
				goto end_switch3;;
			}
		}
		if ((v == LevelEditorEntityParticleSystem_Emitter::NONE) || (v == LevelEditorEntityParticleSystem_Emitter::POINT_PARTICLE_EMITTER)) {
			{
				particleSystemEmitters->getController()->setValue(value->set(EMITTER_POINTPARTICLEEMITTER));
				particleSystemEmitter->getActiveConditions()->add(EMITTER_POINTPARTICLEEMITTER);
				auto emitter = particleSystem->getPointParticleEmitter();
				ppeCount->getController()->setValue(value->set(emitter->getCount()));
				ppeLifeTime->getController()->setValue(value->set(static_cast< int32_t >(emitter->getLifeTime())));
				ppeLifeTimeRnd->getController()->setValue(value->set(static_cast< int32_t >(emitter->getLifeTimeRnd())));
				ppeMass->getController()->setValue(value->set(emitter->getMass(), 4));
				ppeMassRnd->getController()->setValue(value->set(emitter->getMassRnd(), 4));
				ppePosition->getController()->setValue(value->set(Tools::formatVector3(emitter->getPosition())));
				ppeVelocity->getController()->setValue(value->set(Tools::formatVector3(emitter->getVelocity())));
				ppeVelocityRnd->getController()->setValue(value->set(Tools::formatVector3(emitter->getVelocityRnd())));
				ppeColorStart->getController()->setValue(value->set(Tools::formatColor4(emitter->getColorStart())));
				ppeColorEnd->getController()->setValue(value->set(Tools::formatColor4(emitter->getColorEnd())));
				goto end_switch3;;
			}
		}
		if ((v == LevelEditorEntityParticleSystem_Emitter::NONE) || (v == LevelEditorEntityParticleSystem_Emitter::POINT_PARTICLE_EMITTER) || (v == LevelEditorEntityParticleSystem_Emitter::BOUNDINGBOX_PARTICLE_EMITTER)) {
			{
				particleSystemEmitters->getController()->setValue(value->set(EMITTER_BOUNDINGBOXPARTICLEEMITTER));
				particleSystemEmitter->getActiveConditions()->add(EMITTER_BOUNDINGBOXPARTICLEEMITTER);
				auto emitter = particleSystem->getBoundingBoxParticleEmitters();
				bbpeCount->getController()->setValue(value->set(emitter->getCount()));
				bbpeLifeTime->getController()->setValue(value->set(static_cast< int32_t >(emitter->getLifeTime())));
				bbpeLifeTimeRnd->getController()->setValue(value->set(static_cast< int32_t >(emitter->getLifeTimeRnd())));
				bbpeMass->getController()->setValue(value->set(emitter->getMass(), 4));
				bbpeMassRnd->getController()->setValue(value->set(emitter->getMassRnd(), 4));
				bbpeVelocity->getController()->setValue(value->set(Tools::formatVector3(emitter->getVelocity())));
				bbpeVelocityRnd->getController()->setValue(value->set(Tools::formatVector3(emitter->getVelocityRnd())));
				bbpeColorStart->getController()->setValue(value->set(Tools::formatColor4(emitter->getColorStart())));
				bbpeColorEnd->getController()->setValue(value->set(Tools::formatColor4(emitter->getColorEnd())));
				bbpeObbCenter->getController()->setValue(value->set(Tools::formatVector3(emitter->getObbCenter())));
				bbpeObbHalfextension->getController()->setValue(value->set(Tools::formatVector3(emitter->getObbHalfextension())));
				Vector3 rotation;
				Matrix4x4 rotationMatrix;
				rotationMatrix.identity();
				rotationMatrix.setAxes(emitter->getObbAxis0(), emitter->getObbAxis1(), emitter->getObbAxis2());
				rotationMatrix.computeEulerAngles(rotation);
				bbpeObbRotationX->getController()->setValue(value->set(Tools::formatFloat(rotation.getX())));
				bbpeObbRotationY->getController()->setValue(value->set(Tools::formatFloat(rotation.getY())));
				bbpeObbRotationZ->getController()->setValue(value->set(Tools::formatFloat(rotation.getZ())));
				goto end_switch3;;
			}
		}
		if ((v == LevelEditorEntityParticleSystem_Emitter::NONE) || (v == LevelEditorEntityParticleSystem_Emitter::POINT_PARTICLE_EMITTER) || (v == LevelEditorEntityParticleSystem_Emitter::BOUNDINGBOX_PARTICLE_EMITTER) || (v == LevelEditorEntityParticleSystem_Emitter::CIRCLE_PARTICLE_EMITTER)) {
			{
				particleSystemEmitters->getController()->setValue(value->set(EMITTER_CIRCLEPARTICLEEMITTER));
				particleSystemEmitter->getActiveConditions()->add(EMITTER_CIRCLEPARTICLEEMITTER);
				auto emitter = particleSystem->getCircleParticleEmitter();
				cpeCount->getController()->setValue(value->set(emitter->getCount()));
				cpeLifeTime->getController()->setValue(value->set(static_cast< int32_t >(emitter->getLifeTime())));
				cpeLifeTimeRnd->getController()->setValue(value->set(static_cast< int32_t >(emitter->getLifeTimeRnd())));
				cpeMass->getController()->setValue(value->set(emitter->getMass(), 4));
				cpeMassRnd->getController()->setValue(value->set(emitter->getMassRnd(), 4));
				cpeVelocity->getController()->setValue(value->set(Tools::formatVector3(emitter->getVelocity())));
				cpeVelocityRnd->getController()->setValue(value->set(Tools::formatVector3(emitter->getVelocityRnd())));
				cpeColorStart->getController()->setValue(value->set(Tools::formatColor4(emitter->getColorStart())));
				cpeColorEnd->getController()->setValue(value->set(Tools::formatColor4(emitter->getColorEnd())));
				cpeCenter->getController()->setValue(value->set(Tools::formatVector3(emitter->getCenter())));
				cpeRadius->getController()->setValue(value->set(emitter->getRadius(), 4));
				Vector3 tmpAxis;
				Vector3 rotation;
				Matrix4x4 rotationMatrix;
				rotationMatrix.identity();
				rotationMatrix.setAxes(emitter->getAxis0(), Vector3::computeCrossProduct(emitter->getAxis0(), emitter->getAxis1(), tmpAxis), emitter->getAxis1());
				rotationMatrix.computeEulerAngles(rotation);
				cpeRotationX->getController()->setValue(value->set(Tools::formatFloat(rotation.getX())));
				cpeRotationY->getController()->setValue(value->set(Tools::formatFloat(rotation.getY())));
				cpeRotationZ->getController()->setValue(value->set(Tools::formatFloat(rotation.getZ())));
				goto end_switch3;;
			}
		}
		if ((v == LevelEditorEntityParticleSystem_Emitter::NONE) || (v == LevelEditorEntityParticleSystem_Emitter::POINT_PARTICLE_EMITTER) || (v == LevelEditorEntityParticleSystem_Emitter::BOUNDINGBOX_PARTICLE_EMITTER) || (v == LevelEditorEntityParticleSystem_Emitter::CIRCLE_PARTICLE_EMITTER) || (v == LevelEditorEntityParticleSystem_Emitter::CIRCLE_PARTICLE_EMITTER_PLANE_VELOCITY)) {
			{
				particleSystemEmitters->getController()->setValue(value->set(EMITTER_CIRCLEPARTICLEEMITTERPLANEVELOCITY));
				particleSystemEmitter->getActiveConditions()->add(EMITTER_CIRCLEPARTICLEEMITTERPLANEVELOCITY);
				auto emitter = particleSystem->getCircleParticleEmitterPlaneVelocity();
				cpepvCount->getController()->setValue(value->set(emitter->getCount()));
				cpepvLifeTime->getController()->setValue(value->set(static_cast< int32_t >(emitter->getLifeTime())));
				cpepvLifeTimeRnd->getController()->setValue(value->set(static_cast< int32_t >(emitter->getLifeTimeRnd())));
				cpepvMass->getController()->setValue(value->set(emitter->getMass(), 4));
				cpepvMassRnd->getController()->setValue(value->set(emitter->getMassRnd(), 4));
				cpepvVelocity->getController()->setValue(value->set(emitter->getVelocity(), 4));
				cpepvVelocityRnd->getController()->setValue(value->set(emitter->getVelocityRnd(), 4));
				cpepvColorStart->getController()->setValue(value->set(Tools::formatColor4(emitter->getColorStart())));
				cpepvColorEnd->getController()->setValue(value->set(Tools::formatColor4(emitter->getColorEnd())));
				cpepvCenter->getController()->setValue(value->set(Tools::formatVector3(emitter->getCenter())));
				cpepvRadius->getController()->setValue(value->set(emitter->getRadius(), 4));
				Vector3 tmpAxis;
				Vector3 rotation;
				Matrix4x4 rotationMatrix;
				rotationMatrix.identity();
				rotationMatrix.setAxes(emitter->getAxis0(), Vector3::computeCrossProduct(emitter->getAxis0(), emitter->getAxis1(), tmpAxis), emitter->getAxis1());
				rotationMatrix.computeEulerAngles(rotation);
				cpepvRotationX->getController()->setValue(value->set(Tools::formatFloat(rotation.getX())));
				cpepvRotationY->getController()->setValue(value->set(Tools::formatFloat(rotation.getY())));
				cpepvRotationZ->getController()->setValue(value->set(Tools::formatFloat(rotation.getZ())));
				goto end_switch3;;
			}
		}
		if ((v == LevelEditorEntityParticleSystem_Emitter::NONE) || (v == LevelEditorEntityParticleSystem_Emitter::POINT_PARTICLE_EMITTER) || (v == LevelEditorEntityParticleSystem_Emitter::BOUNDINGBOX_PARTICLE_EMITTER) || (v == LevelEditorEntityParticleSystem_Emitter::CIRCLE_PARTICLE_EMITTER) || (v == LevelEditorEntityParticleSystem_Emitter::CIRCLE_PARTICLE_EMITTER_PLANE_VELOCITY) || (v == LevelEditorEntityParticleSystem_Emitter::SPHERE_PARTICLE_EMITTER)) {
			{
				particleSystemEmitters->getController()->setValue(value->set(EMITTER_SPHEREPARTICLEEMITTER));
				particleSystemEmitter->getActiveConditions()->add(EMITTER_SPHEREPARTICLEEMITTER);
				auto emitter = particleSystem->getSphereParticleEmitter();
				speCount->getController()->setValue(value->set(emitter->getCount()));
				speLifeTime->getController()->setValue(value->set(static_cast< int32_t >(emitter->getLifeTime())));
				speLifeTimeRnd->getController()->setValue(value->set(static_cast< int32_t >(emitter->getLifeTimeRnd())));
				speMass->getController()->setValue(value->set(emitter->getMass(), 4));
				speMassRnd->getController()->setValue(value->set(emitter->getMassRnd(), 4));
				speVelocity->getController()->setValue(value->set(Tools::formatVector3(emitter->getVelocity())));
				speVelocityRnd->getController()->setValue(value->set(Tools::formatVector3(emitter->getVelocityRnd())));
				speColorStart->getController()->setValue(value->set(Tools::formatColor4(emitter->getColorStart())));
				speColorEnd->getController()->setValue(value->set(Tools::formatColor4(emitter->getColorEnd())));
				speCenter->getController()->setValue(value->set(Tools::formatVector3(emitter->getCenter())));
				speRadius->getController()->setValue(value->set(emitter->getRadius(), 4));
				goto end_switch3;;
			}
		}
		if (((v == LevelEditorEntityParticleSystem_Emitter::NONE) || (v == LevelEditorEntityParticleSystem_Emitter::POINT_PARTICLE_EMITTER) || (v == LevelEditorEntityParticleSystem_Emitter::BOUNDINGBOX_PARTICLE_EMITTER) || (v == LevelEditorEntityParticleSystem_Emitter::CIRCLE_PARTICLE_EMITTER) || (v == LevelEditorEntityParticleSystem_Emitter::CIRCLE_PARTICLE_EMITTER_PLANE_VELOCITY) || (v == LevelEditorEntityParticleSystem_Emitter::SPHERE_PARTICLE_EMITTER) || ((v != LevelEditorEntityParticleSystem_Emitter::NONE) && (v != LevelEditorEntityParticleSystem_Emitter::POINT_PARTICLE_EMITTER) && (v != LevelEditorEntityParticleSystem_Emitter::BOUNDINGBOX_PARTICLE_EMITTER) && (v != LevelEditorEntityParticleSystem_Emitter::CIRCLE_PARTICLE_EMITTER) && (v != LevelEditorEntityParticleSystem_Emitter::CIRCLE_PARTICLE_EMITTER_PLANE_VELOCITY) && (v != LevelEditorEntityParticleSystem_Emitter::SPHERE_PARTICLE_EMITTER)))) {
			Console::println(
				string(
					"ParticleSystemScreenController::onParticleSystemEmitterApply(): unknown particle system emitter '" +
					particleSystem->getEmitter()->getName() +
					"'"
				)
			);
		}
		end_switch3:;
	}

	view->initParticleSystemRequest();
}

void ParticleSystemScreenController::onParticleSystemLoad()
{
	vector<string> extensions = {
		"tps"
	};
	view->getPopUpsViews()->getFileDialogScreenController()->show(
		particleSystemPath->getPath(),
		"Load from: ",
		&extensions,
		view->getFileName(),
		new ParticleSystemScreenController_onParticleSystemLoad_2(this)
	);
}

void ParticleSystemScreenController::onEntitySave()
{
	auto fileName = view->getEntity()->getEntityFileName();
	if (fileName.length() == 0) {
		fileName = "untitle.tps";
	}
	fileName = Tools::getFileName(fileName);
	vector<string> extensions = {
		"tps"
	};
	view->getPopUpsViews()->getFileDialogScreenController()->show(
		particleSystemPath->getPath(),
		"Save from: ",
		&extensions,
		fileName,
		new ParticleSystemScreenController_onEntitySave_3(this)
);
}

void ParticleSystemScreenController::onParticleSystemReload()
{
	view->reloadFile();
}

void ParticleSystemScreenController::saveFile(const string& pathName, const string& fileName) /* throws(Exception) */
{
	view->saveFile(pathName, fileName);
}

void ParticleSystemScreenController::loadFile(const string& pathName, const string& fileName) /* throws(Exception) */
{
	view->loadFile(pathName, fileName);
}

void ParticleSystemScreenController::showErrorPopUp(const string& caption, const string& message)
{
	view->getPopUpsViews()->getInfoDialogScreenController()->show(caption, message);
}

void ParticleSystemScreenController::onValueChanged(GUIElementNode* node)
{
	entityBaseSubScreenController->onValueChanged(node, view->getEntity());
}

void ParticleSystemScreenController::onActionPerformed(GUIActionListener_Type* type, GUIElementNode* node)
{
	entityBaseSubScreenController->onActionPerformed(type, node, view->getEntity());
	entityDisplaySubScreenController->onActionPerformed(type, node);
	entityBoundingVolumeSubScreenController->onActionPerformed(type, node, view->getEntity());
	{
		auto v = type;
		if ((v == GUIActionListener_Type::PERFORMED)) {
			{
				if (node->getId().compare("button_entity_load") == 0) {
					onParticleSystemLoad();
				} else
				if (node->getId().compare("button_entity_reload") == 0) {
					onParticleSystemReload();
				} else
				if (node->getId().compare("button_entity_save") == 0) {
					onEntitySave();
				} else
				if (node->getId().compare("button_ps_type_apply") == 0) {
					onParticleSystemTypeApply();
				} else
				if (node->getId().compare("button_ops_apply") == 0 || node->getId().compare("button_pps_type_apply") == 0) {
					onParticleSystemTypeDataApply();
				} else
				if (node->getId().compare("button_emitter_apply") == 0) {
					onParticleSystemEmitterApply();
				} else
				if (node->getId().compare("button_ppe_emitter_apply") == 0 || node->getId().compare("button_bbpe_emitter_apply") == 0 || node->getId().compare("button_cpe_emitter_apply") == 0 || node->getId().compare("button_cpepv_emitter_apply") == 0 || node->getId().compare("button_spe_emitter_apply") == 0) {
					onParticleSystemEmitterDataApply();
				} else
				if (node->getId().compare("button_ops_model_file") == 0) {
					vector<string> extensions = {
						"dae",
						"tm"
					};
					view->getPopUpsViews()->getFileDialogScreenController()->show(
						modelPath->getPath(),
						"Load from: ",
						&extensions,
						"",
						new ParticleSystemScreenController_onActionPerformed_4(this)
					);
				} else {
					Console::println(
						string(
							"ModelViewerScreenController::onActionPerformed()::unknown, type='" +
							type->getName() +
							"', id = '" +
							node->getId() +
							"'" +
							", name = '" +
							node->getName() +
							"'"
						)
					);
				}
				goto end_switch4;;
			}		}
		if ((v == GUIActionListener_Type::PERFORMED) || (v == GUIActionListener_Type::PERFORMING)) {
			{
				goto end_switch4;;
			}
		}
		end_switch4:;
	}

}
