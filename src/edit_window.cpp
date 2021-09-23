#include <iostream>
#include "../header/edit_window.hpp"
#include "../data.h"
#include "gtkmm/textbuffer.h"

EditWindow::EditWindow(BaseObjectType* cobject,
	const Glib::RefPtr<Gtk::Builder>& m_refGlade) : Gtk::Window(cobject)
{
	m_refGlade->get_widget("StatusBar", this->statusBar);
	m_refGlade->get_widget("EditWindowBodyView", this->_view);
    m_refGlade->get_widget("EditWindowBodyTitle", this->_title);
	m_refGlade->get_widget("SaveNoteButton", this->_saveNoteButton);

	this->_title->property_xalign() = 0.5f;

    Gtk::Window::set_resizable(false);
	Gtk::Window::signal_show().connect(sigc::mem_fun(this, &EditWindow::signal_show));

	Gtk::Window::signal_delete_event().connect(sigc::mem_fun(this, &EditWindow::signal_hide));

	this->_saveNoteButton->signal_clicked().connect(sigc::mem_fun(this, &EditWindow::signal_save));
}

void EditWindow::signal_show() noexcept
{
	this->_dispatcher->handler()->event(this->get_window_id(), Event::SHOW);
}

bool EditWindow::signal_hide(GdkEventAny* event) noexcept
{
	this->_dispatcher->handler()->event(this->get_window_id(), Event::HIDE);
	return true;
}

void EditWindow::set_application_ref(Glib::RefPtr<Gtk::Application> app) noexcept
{
    this->_app = app;
}

void EditWindow::set_window_size(int width, int height) noexcept
{
    Gtk::Window::set_default_size(width, height);
}

void EditWindow::set_window_css_file_path(std::string_view path) noexcept
{
    auto cssProvider = Gtk::CssProvider::create();
    cssProvider->load_from_path(path.data());

	Gtk::Window::get_style_context()->add_provider_for_screen(Gtk::Window::get_screen(), cssProvider, GTK_STYLE_PROVIDER_PRIORITY_USER);
}

void EditWindow::open_window()
{
    this->_app->add_window(*this);
	Gtk::Window::show();
}

void EditWindow::close_window()
{
    this->_app->remove_window(*this);
	Gtk::Window::hide();
}

void EditWindow::clear_window() noexcept
{
	this->_title->set_text("");
	this->_view->get_buffer()->set_text("");
}

void EditWindow::modal(bool flag) noexcept
{
	Gtk::Window::set_modal(flag);
}

void EditWindow::set_window_title(std::string_view title) noexcept
{
	Gtk::Window::set_title(title.data());
}

std::string EditWindow::get_window_title() const noexcept
{
	return Gtk::Window::get_title();
}

void EditWindow::set_status_message(std::string_view status) noexcept
{
	if(this->statusBar) this->statusBar->set_text(status.data());
}

void EditWindow::set_window_id(std::string_view name) noexcept
{
	Gtk::Window::set_name(name.data());
}

std::string EditWindow::get_window_id() const noexcept
{
	return Gtk::Window::get_name();
}

void EditWindow::set_event_dispatcher(std::shared_ptr<IDispatcher> dispatcher) noexcept
{
	this->_dispatcher = std::move(dispatcher);
}

void EditWindow::signal_save() noexcept
{
	if(0 == this->_title->get_text_length())
	{
		this->set_status_message("Title is empty");
		this->_title->get_style_context()->add_class("error");
		return;
	}

	this->_buffer.title = this->_title->get_text();
    auto viewBuffer = this->_view->get_buffer();
	this->_buffer.note = std::string(viewBuffer->get_text().data(), viewBuffer->get_text().length());
	this->_dispatcher->handler()->event(this->get_window_id(), Event::CHANGE, this->_buffer);
	this->_dispatcher->handler()->event(this->get_window_id(), Event::HIDE);
}

void EditWindow::show_data_in_window(const Data& value) noexcept
{
    this->_title->set_text(value.title);
    this->_title->set_position(this->_title->get_text_length());
	auto buffer = Gtk::TextBuffer::create();
	buffer->set_text(value.note);
	this->_view->set_buffer(buffer);
	this->_buffer = value;
}

