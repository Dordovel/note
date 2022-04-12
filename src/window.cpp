#include "../header/window.h"

#include <algorithm>
#include <iostream>
#include <cstddef>

#include <gtkmm-3.0/gtkmm/dialog.h>
#include "gdkmm/rgba.h"
#include "gtkmm/enums.h"
#include "gtkmm/object.h"

#include "../window_types.hpp"
#include "../header/row.h"
#include "../header/grid.h"

Window::Window(BaseObjectType* cobject,
	const Glib::RefPtr<Gtk::Builder>& m_refGlade) : Gtk::Window(cobject)
{
    Gtk::Window::set_resizable(false);

	m_refGlade->get_widget("View", this->_view);
	m_refGlade->get_widget("AddNewItemButton", this->_addNewItemButton);
	m_refGlade->get_widget("NotesButtonEdit", this->_editButton);
	m_refGlade->get_widget("NotesButtonDelete", this->_deleteButton);
	m_refGlade->get_widget("SaveNoteButton", this->_saveNoteButton);
	m_refGlade->get_widget("StatusBar", this->statusBar);
	m_refGlade->get_widget("ScrollWindow", this->_scrolledWindow);
	m_refGlade->get_widget("GridButton", this->_gridButton);
	m_refGlade->get_widget("ListButton", this->_listButton);

	if(this->statusBar)
	{
	}

	this->change_view("list");

	this->_addNewItemButton->signal_clicked().connect(sigc::mem_fun(this, &Window::signal_add_button_click));
	this->_saveNoteButton->signal_clicked().connect(sigc::mem_fun(this, &Window::signal_save_button_click));
	this->_editButton->signal_clicked().connect(sigc::mem_fun(this, &Window::signal_edit_button_click));
	this->_deleteButton->signal_clicked().connect(sigc::mem_fun(this, &Window::signal_delete_button_click));
	this->_gridButton->signal_clicked().connect(sigc::mem_fun(this, &Window::signal_change_view_to_grid));
	this->_listButton->signal_clicked().connect(sigc::mem_fun(this, &Window::signal_change_view_to_list));

	Gtk::Window::signal_show().connect(sigc::mem_fun(this, &Window::signal_show_window));
	Gtk::Window::signal_delete_event().connect(sigc::mem_fun(this, &Window::signal_hide_window));

	this->_editButton->set_visible(false);
	this->_deleteButton->set_visible(false);
}

void Window::change_view(std::string_view component)
{
	auto child = this->_view->get_children();
	for(auto val : child) this->_view->remove();

	if(component == "list")
	{
		auto newComponent = Gtk::manage(new ListRow);
		newComponent->set_event_handler(this);
		this->_view->add(*newComponent);
		this->_component = newComponent;
	}
	else if(component == "grid")
	{
		auto newComponent = Gtk::manage(new GridRow);
		newComponent->set_event_handler(this);
		this->_view->add(*newComponent);
		this->_component = newComponent;
	}
}

void Window::signal_change_view_to_list() noexcept
{
	this->change_view("list");
	this->_eventDispatcher->handler()->event(this->id(), CoreEventTypes::UPDATE);
}

void Window::signal_change_view_to_grid() noexcept
{
	this->change_view("grid");
	this->_eventDispatcher->handler()->event(this->id(), CoreEventTypes::UPDATE);
}

void Window::signal_show_window() noexcept
{
	this->_eventDispatcher->handler()->event(this->id(), CoreEventTypes::SHOW);
}

bool Window::signal_hide_window(GdkEventAny* event) noexcept
{
	this->_eventDispatcher->handler()->event(this->id(), CoreEventTypes::HIDE);
	return true;
}

void Window::set_application_ref(Glib::RefPtr<Gtk::Application> app) noexcept
{
    this->_app = app;
}

void Window::set_size(int width, int height) noexcept
{
    Gtk::Window::set_default_size(width, height);
}

void Window::set_path_to_css_file(std::string_view path) noexcept
{
    auto cssProvider = Gtk::CssProvider::create();
    cssProvider->load_from_path(path.data());

	Gtk::Window::get_style_context()->add_provider_for_screen(Gtk::Window::get_screen(), cssProvider, GTK_STYLE_PROVIDER_PRIORITY_USER);
}

void Window::open()
{
    this->_app->add_window(*this);
	Gtk::Window::show();
}

void Window::close()
{
    this->_app->remove_window(*this);
	Gtk::Window::hide();
}

void Window::clear() noexcept
{
	this->_component->clear();
}

void Window::modal(bool flag) noexcept
{
	Gtk::Window::set_modal(flag);
}

void Window::set_title(std::string_view title) noexcept
{
	Gtk::Window::set_title(title.data());
}

std::string Window::title() const noexcept
{
	return Gtk::Window::get_title();
}

void Window::set_info_message(std::string_view status) noexcept
{
	if(this->statusBar) this->statusBar->set_text(status.data());
}

void Window::set_id(std::string_view name) noexcept
{
	Gtk::Window::set_name(name.data());
}

std::string Window::id() const noexcept
{
	return Gtk::Window::get_name();
}

void Window::set_event_dispatcher(std::shared_ptr<ICoreDispatcher> dispather) noexcept
{
	this->_eventDispatcher = std::move(dispather);
}

void Window::event(ComponentEventTypes type, std::size_t index) noexcept
{
	switch(type)
	{
		case ComponentEventTypes::ACTIVATE:
			this->_eventDispatcher->handler()->event(this->id(), CoreEventTypes::ACTIVATE, index);
		break;

		case ComponentEventTypes::DEACTIVATE:
			this->_eventDispatcher->handler()->event(this->id(), CoreEventTypes::DEACTIVATE, index);
		break;

		case ComponentEventTypes::SELECT:
			this->_deleteButton->set_visible(true);
			this->_editButton->set_visible(true);
		break;

		default: break;
	}
}

void Window::signal_add_button_click() noexcept
{
	this->_eventDispatcher->handler()->event(this->id(), CoreEventTypes::INSERT);
}

void Window::signal_save_button_click() noexcept
{
    this->_eventDispatcher->handler()->event(this->id(), CoreEventTypes::SAVE);
}

void Window::signal_edit_button_click() noexcept
{
	int selectId = this->_component->get_selected_item();
	if(selectId < 0) return;

    this->_eventDispatcher->handler()->event(this->id(), CoreEventTypes::OPEN, selectId, WindowType::EDIT);
}

void Window::signal_delete_button_click() noexcept
{
	int selectId = this->_component->get_selected_item();
	if(selectId < 0) return;

	this->_eventDispatcher->handler()->event(this->id(), CoreEventTypes::DELETE, selectId);
}

void Window::print(const Block& value) noexcept
{
	this->_component->add(value);
	this->_view->show_all_children(false);
	auto vAdjustment = this->_scrolledWindow->get_vadjustment();
	vAdjustment->set_value(vAdjustment->get_upper());
}
