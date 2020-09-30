#ifndef MAIN_WINDOW
#define MAIN_WINDOW

#include "../interface/window.h"

#include <gtkmm-3.0/gtkmm/window.h>
#include <gtkmm-3.0/gtkmm/listbox.h>
#include <gtkmm-3.0/gtkmm/builder.h>
#include <glibmm-2.4/glibmm/refptr.h>
#include <gtkmm-3.0/gtkmm/button.h>
#include <gtkmm-3.0/gtkmm/cssprovider.h>
#include <gtkmm-3.0/gtkmm/application.h>

class Window final : public virtual Gtk::Window, public IWindow
{
	private:
		std::shared_ptr<IDispatcher> _dispatcher;
        Gtk::ListBox* _listBox;
        Glib::RefPtr<Gdk::Pixbuf> _editIcon;
		Glib::RefPtr<Gdk::Pixbuf> _deleteIcon;
		Glib::RefPtr<Gtk::CssProvider> _cssEntry;
		Glib::RefPtr<Gdk::Screen> _screen;
		Glib::RefPtr<Gtk::StyleContext> _styleContext;
        Glib::RefPtr<Gtk::Application> _app;

        void selected_row(Gtk::ListBoxRow* row) noexcept;
		void button_edit_click(Gtk::Button* button, Gtk::ListBoxRow* row) noexcept;
        void button_delete_click(Gtk::Button* button, Gtk::ListBoxRow* row) noexcept;
        void toggle_check(Gtk::Button* button, Gtk::ListBoxRow* row) noexcept;


	public:
		Window(BaseObjectType* cobject, 
			const Glib::RefPtr<Gtk::Builder>& m_refGlade);

		void show() override;
		void hide() override;

		void app(Glib::RefPtr<Gtk::Application> app) noexcept;

		void set_title(std::string_view title) noexcept override;
		std::string get_title() const noexcept override;

		void set_name(std::string_view name) noexcept override;
		std::string get_name() const noexcept override;

		void set_dispatcher(const std::shared_ptr<IDispatcher>& dispather) noexcept;

		void add_data(std::vector<struct data> data) noexcept override;
		void add_data(struct data) noexcept override;

		std::vector<struct data> get_data() const noexcept override;

        void set_style(std::string_view path) noexcept override;

        void set_size(int width = -1, int height = -1) noexcept override;

        ~Window() = default;

};

#endif //MAIN_WINDOW
