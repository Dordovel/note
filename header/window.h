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

        void signal_show() noexcept;
        bool signal_hide(GdkEventAny* event) noexcept;

		void row_button_edit_click_signal(std::size_t rowIndex) noexcept;
        void row_button_delete_click_signal(std::size_t rowIndex) noexcept;
        void row_checkbutton_click_signal(Gtk::CheckButton* button) noexcept;
		void button_add_new_item_to_listbox_signal() noexcept;
		void button_save_listbox_changes_signal() noexcept;

		Gtk::ListBoxRow* create_new_row(const Data& value) noexcept;


    protected:
        virtual class Gtk::Grid* create_tool_buttons(size_t rowIndex);


	public:
		Window(BaseObjectType* cobject, 
			const Glib::RefPtr<Gtk::Builder>& m_refGlade);

		Window(Window&& window) = default;
		Window& operator= (Window&& window) = default;

		void open_window() override;
		void close_window() override;

		void modal(bool flag) noexcept override;

		void set_application_ref(Glib::RefPtr<Gtk::Application> app) noexcept;

		void set_window_title(std::string_view title) noexcept override;
		std::string get_window_title() const noexcept override;

		void set_status_message(std::string_view status) noexcept override;

		void set_window_id(std::string_view name) noexcept override;
		std::string get_window_id() const noexcept override;

        virtual void set_event_dispatcher(std::shared_ptr<IDispatcher> dispatcher) noexcept;

		void show_data_in_window(const Data& value) noexcept override;

        void set_window_css_file_path(std::string_view path) noexcept override;

        void set_window_size(int width = -1, int height = -1) noexcept override;

		void clear_window() noexcept override;

        virtual ~Window() = default;

};

#endif //WINDOW
