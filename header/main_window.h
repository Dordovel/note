#ifndef MAIN_WINDOW
#define MAIN_WINDOW

#include "./window.h"

class MainWindow : public Window
{
    private:
        std::shared_ptr<ICoreDispatcher> _dispatcher;
		Gtk::Button* _openButton;

		void signal_open_button_click() noexcept;

	public:
		MainWindow(BaseObjectType* cobject, 
			const Glib::RefPtr<Gtk::Builder>& m_refGlade);

		void set_event_dispatcher(std::shared_ptr<ICoreDispatcher> dispather) noexcept override;

		~MainWindow() = default;

};

#endif //MAIN_WINDOW
