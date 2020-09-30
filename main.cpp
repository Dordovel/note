#include <gtkmm-3.0/gtkmm.h> 
#include "./header/window.h"
#include "./header/core.h"
#include "./header/dispatcher.h"
#include "./data.h"

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
	window->set_name("Main");
	window->set_dispatcher(dispather);
	window->set_style("./resource/style/entry_border.css");

	window->add_data({"1", "Hello", true});
    window->add_data({"2", "jsdjklfajsdfjkashgfhasklfaskdgfawehfklasdfhkgasdfuihasdjkl", false});
    window->add_data({"3", "Hello", true});
    window->add_data({"4", "Hello", true});

	core->register_window(window->get_name(), window);

	app->run(*window);

	delete window;

	return 0;
}
