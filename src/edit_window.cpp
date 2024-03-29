#include "../header/edit_window.hpp"
#include "../data.h"
#include "gtkmm/textbuffer.h"

EditWindow::EditWindow(BaseObjectType* cobject,
	const Glib::RefPtr<Gtk::Builder>& m_refGlade) : Gtk::Window(cobject)
{
	m_refGlade->get_widget("StatusBar", this->_status_bar);
	m_refGlade->get_widget("EditWindowBodyView", this->_view);
    m_refGlade->get_widget("EditWindowBodyTitle", this->_title);
	m_refGlade->get_widget("SaveNoteButton", this->_save_note_button);

	this->_title->property_xalign() = 0.5f;

    Gtk::Window::set_resizable(false);
	Gtk::Window::signal_show().connect(sigc::mem_fun(this, &EditWindow::signal_show_window));

	Gtk::Window::signal_delete_event().connect(sigc::mem_fun(this, &EditWindow::signal_hide_window));

	this->_save_note_button->signal_clicked().connect(sigc::mem_fun(this, &EditWindow::signal_save_button_click));
}

void EditWindow::signal_show_window() noexcept
{
	this->_dispatcher->handler()->event(this->id(), CoreEventTypes::SHOW);
}

bool EditWindow::signal_hide_window(GdkEventAny* event) noexcept
{
	this->_dispatcher->handler()->event(this->id(), CoreEventTypes::HIDE);
	return true;
}

void EditWindow::set_application_ref(Glib::RefPtr<Gtk::Application> app) noexcept
{
    this->_app = app;
}

void EditWindow::set_size(int width, int height) noexcept
{
    Gtk::Window::set_default_size(width, height);
}

void EditWindow::set_path_to_css_file(std::string_view path) noexcept
{
    auto cssProvider = Gtk::CssProvider::create();
    cssProvider->load_from_path(path.data());

	Gtk::Window::get_style_context()->add_provider_for_screen(Gtk::Window::get_screen(), cssProvider, GTK_STYLE_PROVIDER_PRIORITY_USER);
}

void EditWindow::open()
{
    this->_app->add_window(*this);
	Gtk::Window::show();
}

void EditWindow::close()
{
    this->_app->remove_window(*this);
	Gtk::Window::hide();
}

void EditWindow::clear() noexcept
{
	this->_title->set_text("");
	this->_view->get_buffer()->set_text("");
}

void EditWindow::modal(bool flag) noexcept
{
	Gtk::Window::set_modal(flag);
}

void EditWindow::set_title(std::string_view title) noexcept
{
	Gtk::Window::set_title(title.data());
}

std::string EditWindow::title() const noexcept
{
	return Gtk::Window::get_title();
}

void EditWindow::set_info_message(std::string_view status) noexcept
{
	if(this->_status_bar) this->_status_bar->set_text(status.data());
}

void EditWindow::set_id(std::string_view name) noexcept
{
	Gtk::Window::set_name(name.data());
}

std::string EditWindow::id() const noexcept
{
	return Gtk::Window::get_name();
}

void EditWindow::set_event_dispatcher(std::shared_ptr<ICoreDispatcher> dispatcher) noexcept
{
	this->_dispatcher = std::move(dispatcher);
}

void EditWindow::event(ComponentEventTypes type, std::size_t index) noexcept
{
}

void EditWindow::signal_save_button_click() noexcept
{
	if(0 == this->_title->get_text_length())
	{
		this->set_info_message("Title is empty");
		this->_title->get_style_context()->add_class("error");
		return;
	}

	this->_buffer.set_header(this->_title->get_text().c_str());
    auto viewBuffer = this->_view->get_buffer();
	this->_buffer.set_note(std::string(viewBuffer->get_text().data(), viewBuffer->get_text().length()));
	this->_dispatcher->handler()->event(this->id(), CoreEventTypes::CHANGE, this->_buffer);
	this->_dispatcher->handler()->event(this->id(), CoreEventTypes::HIDE);
}

void EditWindow::print(const Block& value) noexcept
{
    this->_title->set_text(value.get_header());
    this->_title->set_position(this->_title->get_text_length());
	auto buffer = Gtk::TextBuffer::create();
	buffer->set_text(value.get_note());
	this->_view->set_buffer(buffer);
	this->_buffer = value;
}

