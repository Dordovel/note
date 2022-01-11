#ifndef MAIN_WINDOW
#define MAIN_WINDOW

#include "./window.h"

class MainWindow : public Window
{
    private:
        std::shared_ptr<IDispatcher> _dispatcher;
        Glib::RefPtr<Gdk::Pixbuf> _open_icon;
        class Gtk::Grid* tool_buttons(size_t rowIndex) override;

		void signal_open_button_click(std::size_t rowIndex) noexcept;

	public:
		MainWindow(BaseObjectType* cobject, 
			const Glib::RefPtr<Gtk::Builder>& m_refGlade);

		void set_event_dispatcher(std::shared_ptr<IDispatcher> dispather) noexcept
			override;

		~MainWindow() = default;

};

#endif //MAIN_WINDOW
