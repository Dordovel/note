#include <gtkmm-3.0/gtkmm.h>
#include <iostream>
#include "./header/window.h"
#include "./header/core.h"
#include "./header/dispatcher.h"
#include "./data.h"

#include "./header/database.h"

int main()
{
    Glib::RefPtr<Gtk::Application> app = Gtk::Application::create("Test");
	std::shared_ptr<Core> core = std::make_shared<Core>();

	std::shared_ptr<Dispatcher> dispather = std::make_shared<Dispatcher>();
	dispather->set_handle(core);

	Glib::RefPtr<Gtk::Builder> builder = Gtk::Builder::create_from_file("./change.glade");

	Window* window = nullptr;
	builder->get_widget_derived("mainWindow", window);
	window->set_size(400);
	window->set_name("Edit");
	window->set_dispatcher(dispather);
	window->set_style("./resource/style/entry_border.css");

	core->register_window(window->get_name(), window);

	Database database;
	database.connect("./resource/db/note.db");
	//database.query_insert("edit", {"parent_id", "data", "status", "deleted"}, {"1", "Test Value", "0", "0"});
    auto data = database.query_select("edit", {"parent_id", "data", "status", "deleted"}, {"id = 0"});

    for(const auto& row : data)
    {
        for(const auto& colums : row)
        {
            std::cout<<colums<<std::endl;
        }
    }

	app->run(*window);

	delete window;

	return 0;
}
