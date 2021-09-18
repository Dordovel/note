#include <gtkmm-3.0/gtkmm.h>
#include "./header/window.h"
#include "./header/main_window.h"
#include "./header/edit_window.hpp"
#include "./header/core.h"
#include "./header/window_manager.hpp"
#include "window_types.hpp"

int main()
{
    Glib::RefPtr<Gtk::Application> app = Gtk::Application::create("Note");
	std::shared_ptr<IDatabase> database = std::make_shared<Database>();
	database->connect("./resource/db/note.db");

	std::shared_ptr<Core> core = std::make_shared<Core>(database);

	std::shared_ptr<Dispatcher> dispatcher = std::make_shared<Dispatcher>();
	dispatcher->set_handle(core);

	Glib::RefPtr<Gtk::Builder> builder = Gtk::Builder::create();

	Window* gtkListWindowPointer = nullptr;
    builder->add_from_file("./resource/change.glade");
    builder->get_widget_derived("mainWindow", gtkListWindowPointer);
	gtkListWindowPointer->set_size(400);
	gtkListWindowPointer->set_name("list");
	gtkListWindowPointer->set_dispatcher(dispatcher);
	gtkListWindowPointer->set_style("./resource/style/entry_border.css");
	gtkListWindowPointer->app(app);

	MainWindow* gtkMainWindowPointer = nullptr;
	builder->add_from_file("./resource/change.glade");
	builder->get_widget_derived("mainWindow", gtkMainWindowPointer);
	gtkMainWindowPointer->set_size(400);
	gtkMainWindowPointer->set_name("main");
	gtkMainWindowPointer->set_dispatcher(dispatcher);
	gtkMainWindowPointer->set_style("./resource/style/entry_border.css");
    gtkMainWindowPointer->app(app);

	EditWindow* gtkEditWindowPointer = nullptr;
	builder->add_from_file("./resource/change.glade");
	builder->get_widget_derived("EditWindow", gtkEditWindowPointer);
	gtkEditWindowPointer->set_size(400, 400);
	gtkEditWindowPointer->set_name("edit");
	gtkEditWindowPointer->set_dispatcher(dispatcher);
	gtkEditWindowPointer->set_style("./resource/style/entry_border.css");
    gtkEditWindowPointer->app(app);

	std::shared_ptr<Window> gtkListWindow(gtkListWindowPointer);
	std::shared_ptr<EditWindow> gtkEditWindow(gtkEditWindowPointer);
	std::shared_ptr<MainWindow> gtkMainWindow(gtkMainWindowPointer);

	std::unique_ptr<WindowManager> manager = std::make_unique<WindowManager>();
	manager->register_window(gtkListWindow->get_name(), WindowType::LIST, std::move(gtkListWindow));
	manager->register_window(gtkEditWindow->get_name(), WindowType::EDIT, std::move(gtkEditWindow));
	manager->register_window(gtkMainWindow->get_name(), WindowType::MAIN, std::move(gtkMainWindow));

	core->register_manager(std::move(manager));

	app->run(*gtkMainWindowPointer);

	return 0;
}
