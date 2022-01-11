#ifndef WINDOW
#define WINDOW

#include "../interface/window.h"
#include "./dispatcher.h"
#include "gtkmm/checkbutton.h"

#include <gtkmm-3.0/gtkmm/window.h>
#include <gtkmm-3.0/gtkmm/label.h>
#include <gtkmm-3.0/gtkmm/listbox.h>
#include <gtkmm-3.0/gtkmm/listboxrow.h>
#include <gtkmm-3.0/gtkmm/builder.h>
#include <glibmm-2.4/glibmm/refptr.h>
#include <gtkmm-3.0/gtkmm/button.h>
#include <gtkmm-3.0/gtkmm/cssprovider.h>
#include <gtkmm-3.0/gtkmm/application.h>
#include <gtkmm-3.0/gtkmm/grid.h>
#include <gtkmm-3.0/gtkmm/scrolledwindow.h>

class Window : public Gtk::Window, public IWindow
{
	private:
		std::shared_ptr<IDispatcher> _eventDispatcher;
        Glib::RefPtr<Gdk::Pixbuf> _editIcon;
		Glib::RefPtr<Gdk::Pixbuf> _deleteIcon;
        Glib::RefPtr<Gtk::Application> _app;

        Gtk::ListBox* _listBox;
		Gtk::Button* _addNewItemButton;
		Gtk::Button* _saveNoteButton;
		Gtk::Label* statusBar;
		Gtk::ScrolledWindow* _scrolledWindow;

        void signal_show_window() noexcept;
        bool signal_hide_window(GdkEventAny* event) noexcept;

		void signal_edit_button_click(std::size_t rowIndex) noexcept;
        void signal_delete_button_click(std::size_t rowIndex) noexcept;
        void signal_activate_button_click(Gtk::CheckButton* button) noexcept;
		void signal_add_button_click() noexcept;
		void signal_save_button_click() noexcept;

		Gtk::ListBoxRow* create_row(const Data& value) noexcept;


    protected:
        virtual class Gtk::Grid* tool_buttons(size_t rowIndex);


	public:
		Window(BaseObjectType* cobject, 
			const Glib::RefPtr<Gtk::Builder>& m_refGlade);

		Window(Window&& window) = default;
		Window& operator= (Window&& window) = default;

		void open() override;
		void close() override;

		void modal(bool flag) noexcept override;

		void set_application_ref(Glib::RefPtr<Gtk::Application> app) noexcept;

		void set_title(std::string_view title) noexcept override;
		std::string title() const noexcept override;

		void set_info_message(std::string_view status) noexcept override;

		void set_id(std::string_view name) noexcept override;
		std::string id() const noexcept override;

        virtual void set_event_dispatcher(std::shared_ptr<IDispatcher> dispatcher) noexcept;

		void print(const Data& value) noexcept override;

        void set_path_to_css_file(std::string_view path) noexcept override;

        void set_size(int width = -1, int height = -1) noexcept override;

		void clear() noexcept override;

        virtual ~Window() = default;

};

#endif //WINDOW
