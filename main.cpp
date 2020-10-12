#include <gtkmm-3.0/gtkmm.h>
#include <iostream>
#include "./header/window.h"
#include "./header/mainwindow.h"
#include "./header/core.h"
#include "./header/dispatcher.h"

#include "./header/database.h"

int main()
{
    Glib::RefPtr<Gtk::Application> app = Gtk::Application::create("Test");
	std::shared_ptr<IDatabase> database = std::make_shared<Database>();
	database->connect("./resource/db/note.db");

	std::shared_ptr<Core> core = std::make_shared<Core>(database);

	std::shared_ptr<Dispatcher> dispather = std::make_shared<Dispatcher>();
	dispather->set_handle(core);

	Glib::RefPtr<Gtk::Builder> builder = Gtk::Builder::create();

	MainWindow* main = nullptr;
	builder->add_from_file("./change.glade");
	builder->get_widget_derived("mainWindow", main);
	main->set_size(400);
	main->set_name("main");
	main->set_dispatcher(dispather);
	main->set_style("./resource/style/entry_border.css");
    main->app(app);

	Window* edit = nullptr;
    builder->add_from_file("./change.glade");
    builder->get_widget_derived("mainWindow", edit);
	edit->set_size(400);
	edit->set_name("edit");
	edit->set_dispatcher(dispather);
	edit->set_style("./resource/style/entry_border.css");
	edit->app(app);

	core->register_window(main->get_name(), main);
	core->register_window(edit->get_name(), edit);


	app->run(*main);

	delete main;
	delete edit;

	return 0;
}
