#ifndef WINDOW
#define WINDOW

#include "../interface/window.h"

#include <gtkmm-3.0/gtkmm/window.h>
#include <gtkmm-3.0/gtkmm/listbox.h>
#include <gtkmm-3.0/gtkmm/builder.h>
#include <glibmm-2.4/glibmm/refptr.h>
#include <gtkmm-3.0/gtkmm/button.h>
#include <gtkmm-3.0/gtkmm/cssprovider.h>
#include <gtkmm-3.0/gtkmm/application.h>

namespace Gtk
{
    class Grid;
};

class Window : public Gtk::Window, public IWindow
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
		Gtk::Button* _addNewItemButton;
		Gtk::Button* _saveNoteButton;

        void signal_show() noexcept;
        void signal_hide() noexcept;
		void window_hide() noexcept;

		void button_edit_click(Gtk::Button* button, Gtk::ListBoxRow* row) noexcept;
        void button_delete_click(Gtk::Button* button, Gtk::ListBoxRow* row) noexcept;
        void toggle_check(Gtk::Button* button, Gtk::ListBoxRow* row) noexcept;
		void button_add_new_item() noexcept;
		void button_save_note() noexcept;

		Gtk::ListBoxRow* create_new_row(const Data& value) noexcept;


    protected:
        virtual class Gtk::Grid* create_tool_buttons(Gtk::ListBoxRow* row);


	public:
		Window(BaseObjectType* cobject, 
			const Glib::RefPtr<Gtk::Builder>& m_refGlade);

		void show() override;
		void hide() override;

		void modal(bool flag) noexcept override;

		void app(Glib::RefPtr<Gtk::Application> app) noexcept;

		void set_title(std::string_view title) noexcept override;
		std::string get_title() const noexcept override;

		void set_name(std::string_view name) noexcept override;
		std::string get_name() const noexcept override;

        virtual void set_dispatcher(const std::shared_ptr<IDispatcher>& dispather) noexcept;

		void show_data(const Data& value) noexcept override;

        void set_style(std::string_view path) noexcept override;

        void set_size(int width = -1, int height = -1) noexcept override;

        virtual ~Window() = default;

};

#endif //WINDOW
