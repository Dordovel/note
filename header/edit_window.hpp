#ifndef EDIT_WINDOW
#define EDIT_WINDOW

#include "../interface/window.h"
#include "./dispatcher.h"

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
		std::shared_ptr<IDispatcher> _dispatcher;
		Glib::RefPtr<Gtk::CssProvider> _cssEntry;
        Glib::RefPtr<Gtk::Application> _app;

		Gtk::Label* statusBar;
		Gtk::TextView* _view;
		Gtk::Entry* _title;

		Gtk::Button* _saveNoteButton;

        void signal_show() noexcept;
        bool signal_hide(GdkEventAny* event) noexcept;
		void signal_save(struct Data old) noexcept;

	public:
		EditWindow(BaseObjectType* cobject, 
			const Glib::RefPtr<Gtk::Builder>& m_refGlade);

		EditWindow(EditWindow&& window) = default;
		EditWindow& operator= (EditWindow&& window) = default;

		void show() override;
		void hide() override;

		void modal(bool flag) noexcept override;

		void app(Glib::RefPtr<Gtk::Application> app) noexcept;

		void set_title(std::string_view title) noexcept override;
		std::string get_title() const noexcept override;

		void set_status_message(std::string_view status) noexcept override;

		void set_name(std::string_view name) noexcept override;
		std::string get_name() const noexcept override;

        virtual void set_dispatcher(std::shared_ptr<IDispatcher> dispather) noexcept;

		void show_data(const Data& value) noexcept override;

        void set_style(std::string_view path) noexcept override;

        void set_size(int width = -1, int height = -1) noexcept override;

		void clear() noexcept override;
};

#endif //EDIT_WINDOW
