#ifndef EDIT_WINDOW
#define EDIT_WINDOW

#include "../interface/window.h"
#include "./core_dispatcher.h"
#include "../data.h"

#include <gtkmm-3.0/gtkmm/window.h>
#include <glibmm-2.4/glibmm/refptr.h>
#include <gtkmm-3.0/gtkmm/application.h>
#include <gtkmm-3.0/gtkmm/builder.h>
#include <gtkmm-3.0/gtkmm/cssprovider.h>
#include <gtkmm-3.0/gtkmm/label.h>
#include <gtkmm-3.0/gtkmm/entry.h>
#include <gtkmm-3.0/gtkmm/textview.h>
#include <gtkmm-3.0/gtkmm/button.h>

class EditWindow : public Gtk::Window, public IWindow
{
	private:
		std::shared_ptr<ICoreDispatcher> _dispatcher;
		Glib::RefPtr<Gtk::CssProvider> _css_entry;
        Glib::RefPtr<Gtk::Application> _app;

		Gtk::Label* _status_bar;
		Gtk::TextView* _view;
		Gtk::Entry* _title;

		Gtk::Button* _save_note_button;

        void signal_show_window() noexcept;
        bool signal_hide_window(GdkEventAny* event) noexcept;
		void signal_save_button_click() noexcept;

		Data _buffer;

	public:
		EditWindow(BaseObjectType* cobject, 
			const Glib::RefPtr<Gtk::Builder>& m_refGlade);

		EditWindow(EditWindow&& window) = default;
		EditWindow& operator= (EditWindow&& window) = default;

		void open() override;
		void close() override;

		void modal(bool flag) noexcept override;

		void set_application_ref(Glib::RefPtr<Gtk::Application> app) noexcept;

		void set_title(std::string_view title) noexcept override;
		std::string title() const noexcept override;

		void set_info_message(std::string_view status) noexcept override;

		void set_id(std::string_view name) noexcept override;
		std::string id() const noexcept override;

        virtual void set_event_dispatcher(std::shared_ptr<ICoreDispatcher> dispather) noexcept;
		void event(ComponentEventTypes type, std::size_t index) noexcept override;

		void print(const Data& value) noexcept override;

        void set_path_to_css_file(std::string_view path) noexcept override;

        void set_size(int width = -1, int height = -1) noexcept override;

		void clear() noexcept override;
};

#endif //EDIT_WINDOW
