#ifndef WINDOW
#define WINDOW

#include "../interface/window.h"
#include "../interface/component.h"
#include "./core_dispatcher.h"
#include "gtkmm/checkbutton.h"
#include "gtkmm/viewport.h"

#include <gtkmm-3.0/gtkmm/window.h>
#include <gtkmm-3.0/gtkmm/label.h>
#include <gtkmm-3.0/gtkmm/builder.h>
#include <glibmm-2.4/glibmm/refptr.h>
#include <gtkmm-3.0/gtkmm/button.h>
#include <gtkmm-3.0/gtkmm/cssprovider.h>
#include <gtkmm-3.0/gtkmm/application.h>
#include <gtkmm-3.0/gtkmm/grid.h>
#include <gtkmm-3.0/gtkmm/combobox.h>
#include <gtkmm-3.0/gtkmm/scrolledwindow.h>

class Window : public Gtk::Window, public IWindow
{
	private:
		std::shared_ptr<ICoreDispatcher> _eventDispatcher;
        Glib::RefPtr<Gtk::Application> _app;

		Gtk::Button* _addNewItemButton;
		Gtk::Button* _saveNoteButton;
		Gtk::Button* _editButton;
		Gtk::Button* _deleteButton;
		Gtk::Label* statusBar;
		Gtk::ScrolledWindow* _scrolledWindow;
		Gtk::Viewport* _view;

        void signal_show_window() noexcept;
        bool signal_hide_window(GdkEventAny* event) noexcept;

		void signal_edit_button_click() noexcept;
        void signal_delete_button_click() noexcept;
		void signal_add_button_click() noexcept;
		void signal_save_button_click() noexcept;

		void change_view(std::string_view component);
	
	protected:
		IComponent* _component;

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

        virtual void set_event_dispatcher(std::shared_ptr<ICoreDispatcher> dispatcher) noexcept;
		void event(ComponentEventTypes type, std::size_t index) noexcept override;

		void print(const Data& value) noexcept override;

        void set_path_to_css_file(std::string_view path) noexcept override;

        void set_size(int width = -1, int height = -1) noexcept override;

		void clear() noexcept override;

        virtual ~Window() = default;

};

#endif //WINDOW
