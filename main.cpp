#include <gtkmm-3.0/gtkmm.h>
#include "./header/window.h"
#include "./header/main_window.h"
#include "./header/edit_window.hpp"
#include "./header/core.h"
#include "./header/window_manager.hpp"
#include "header/page_factory.h"
#include "window_types.hpp"

int main()
{
    Glib::RefPtr<Gtk::Application> app = Gtk::Application::create("Note");
	std::shared_ptr<IDatabase> database = std::make_shared<Database>();
	database->connect("./resource/db/note.db");

	std::unique_ptr<PageFactory> factory = std::make_unique<PageFactory>(database);
	std::unique_ptr<PageSaver> saver = std::make_unique<PageSaver>(database);

	std::shared_ptr<Core> core = std::make_shared<Core>();

	std::shared_ptr<CoreDispatcher> dispatcher = std::make_shared<CoreDispatcher>();
	dispatcher->set_handler(core);

	Glib::RefPtr<Gtk::Builder> builder = Gtk::Builder::create();

	Window* gtkListWindowPointer = nullptr;
    builder->add_from_file("./resource/main.glade");
    builder->get_widget_derived("mainWindow", gtkListWindowPointer);
	gtkListWindowPointer->set_size(500, 400);
	gtkListWindowPointer->set_id("list");
    gtkListWindowPointer->set_event_dispatcher(dispatcher);
	gtkListWindowPointer->set_path_to_css_file("./resource/style/main.css");
    gtkListWindowPointer->set_application_ref(app);

	MainWindow* gtkMainWindowPointer = nullptr;
	builder->add_from_file("./resource/main.glade");
	builder->get_widget_derived("mainWindow", gtkMainWindowPointer);
	gtkMainWindowPointer->set_size(500, 400);
	gtkMainWindowPointer->set_id("main");
    gtkMainWindowPointer->set_event_dispatcher(dispatcher);
	gtkMainWindowPointer->set_path_to_css_file("./resource/style/main.css");
    gtkMainWindowPointer->set_application_ref(app);

	EditWindow* gtkEditWindowPointer = nullptr;
	builder->add_from_file("./resource/edit.glade");
	builder->get_widget_derived("EditWindow", gtkEditWindowPointer);
	gtkEditWindowPointer->set_size(500, 400);
	gtkEditWindowPointer->set_id("edit");
	gtkEditWindowPointer->set_event_dispatcher(dispatcher);
	gtkEditWindowPointer->set_path_to_css_file("./resource/style/edit.css");
    gtkEditWindowPointer->set_application_ref(app);

	std::shared_ptr<Window> gtkListWindow(gtkListWindowPointer);
	std::shared_ptr<EditWindow> gtkEditWindow(gtkEditWindowPointer);
	std::shared_ptr<MainWindow> gtkMainWindow(gtkMainWindowPointer);

	std::unique_ptr<WindowManager> manager = std::make_unique<WindowManager>();
	manager->register_window(gtkListWindowPointer->id(), WindowType::LIST, std::move(gtkListWindow));
	manager->register_window(gtkEditWindowPointer->id(), WindowType::EDIT, std::move(gtkEditWindow));
	manager->register_window(gtkMainWindowPointer->id(), WindowType::MAIN, std::move(gtkMainWindow));

	core->register_manager(std::move(manager));
	core->register_page_factory(std::move(factory));
	core->register_page_saver(std::move(saver));

	app->run(*gtkMainWindowPointer);

	return 0;
}
