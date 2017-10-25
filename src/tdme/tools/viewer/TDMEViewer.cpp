#include <tdme/tools/viewer/TDMEViewer.h>

#include <cstdlib>
#include <string>

#include <tdme/utils/Time.h>

#include <tdme/engine/Engine.h>
#include <tdme/gui/GUI.h>
#include <tdme/tools/shared/tools/Tools.h>
#include <tdme/tools/shared/views/PopUps.h>
#include <tdme/tools/shared/views/SharedModelViewerView.h>
#include <tdme/tools/shared/views/View.h>
#include <tdme/utils/Console.h>

using std::string;

using tdme::tools::viewer::TDMEViewer;

using tdme::utils::Time;

using tdme::engine::Engine;
using tdme::gui::GUI;
using tdme::tools::shared::tools::Tools;
using tdme::tools::shared::views::PopUps;
using tdme::tools::shared::views::SharedModelViewerView;
using tdme::tools::shared::views::View;
using tdme::utils::Console;

string TDMEViewer::VERSION = "0.9.9";

TDMEViewer* TDMEViewer::instance = nullptr;

TDMEViewer::TDMEViewer()
{
	TDMEViewer::instance = this;
	engine = Engine::getInstance();
	view = nullptr;
	viewInitialized = false;
	viewNew = nullptr;
	popUps = new PopUps();
	modelViewerView = nullptr;
	quitRequested = false;
}

TDMEViewer::~TDMEViewer() {
	delete modelViewerView;
	delete popUps;
}

void TDMEViewer::main(int argc, char** argv)
{
	Console::println(string("TDMEViewer "+ VERSION));
	Console::println(string("Programmed 2014,...,2017 by Andreas Drewke, drewke.net."));
	Console::println();

	TDMEViewer tdmeLevelEditor;
	tdmeLevelEditor.run(argc, argv, "TDMEViewer");
}

TDMEViewer* TDMEViewer::getInstance()
{
	return instance;
}

void TDMEViewer::setView(View* view)
{
	viewNew = view;
}

View* TDMEViewer::getView()
{
	return view;
}

void TDMEViewer::quit()
{
	quitRequested = true;
}

void TDMEViewer::display()
{
	if (viewNew != nullptr) {
		if (view != nullptr && viewInitialized == true) {
			view->deactivate();
			view->dispose();
			viewInitialized = false;
		}
		view = viewNew;
		viewNew = nullptr;
	}
	if (view != nullptr) {
		if (viewInitialized == false) {
			view->initialize();
			view->activate();
			viewInitialized = true;
		}
		view->display();
	}
	engine->display();
	view->display();
	if (quitRequested == true) {
		if (view != nullptr) {
			view->deactivate();
			view->dispose();
		}
		exit(0);
	}
}

void TDMEViewer::dispose()
{
	if (view != nullptr && viewInitialized == true) {
		view->deactivate();
		view->dispose();
		view = nullptr;
	}
	engine->dispose();
	Tools::oseDispose();
}

void TDMEViewer::initialize()
{
	Console::println("initialize");
	engine->initialize();
	setInputEventHandler(engine->getGUI());
	Tools::oseInit();
	popUps->initialize();
	setView(modelViewerView = new SharedModelViewerView(popUps));
}

void TDMEViewer::reshape(int32_t width, int32_t height)
{
	engine->reshape(0, 0, width, height);
}
