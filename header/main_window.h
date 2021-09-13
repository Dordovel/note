#ifndef MAIN_WINDOW
#define MAIN_WINDOW

#include "./window.h"

class MainWindow : public Window
{
    private:
        std::shared_ptr<IDispatcher> _dispatcher;
        Glib::RefPtr<Gdk::Pixbuf> _openIcon;
        class Gtk::Grid* create_tool_buttons(Gtk::ListBoxRow* row) override;

		void button_open_click(Gtk::Button* button, Gtk::ListBoxRow* row) noexcept;

	public:
		MainWindow(BaseObjectType* cobject, 
			const Glib::RefPtr<Gtk::Builder>& m_refGlade);

		void set_dispatcher(std::shared_ptr<IDispatcher> dispather) noexcept
			override;

		~MainWindow() = default;

};

#endif //MAIN_WINDOW
