#include "../header/window.h"

#include "../data.h"
#include "../window_types.hpp"

#include <algorithm>

#include <cstddef>
#include <gtkmm-3.0/gtkmm/box.h>
#include <gtkmm-3.0/gtkmm/button.h>
#include <gtkmm-3.0/gtkmm/entry.h>
#include <gtkmm-3.0/gtkmm/image.h>
#include <gtkmm-3.0/gtkmm/checkbutton.h>
#include <gtkmm-3.0/gtkmm/grid.h>
#include <gtkmm-3.0/gtkmm/dialog.h>

namespace
{
	void remove_row(Gtk::ListBox* list, Gtk::ListBoxRow* row) noexcept
	{
		list->remove(*row);
	}

	void clear_list(Gtk::ListBox* list) noexcept
	{
		auto rows = list->get_children();
		for(auto row : rows)
			list->remove(*row);
	}
}

Window::Window(BaseObjectType* cobject,
	const Glib::RefPtr<Gtk::Builder>& m_refGlade) : Gtk::Window(cobject)
{
    m_refGlade->get_widget("ListBox", this->_listBox);
	m_refGlade->get_widget("AddNewItemButton", this->_addNewItemButton);
	this->_addNewItemButton->signal_clicked().connect(sigc::mem_fun(this, &Window::button_add_new_item));

	m_refGlade->get_widget("SaveNoteButton", this->_saveNoteButton);
	this->_saveNoteButton->signal_clicked().connect(sigc::mem_fun(this, &Window::button_save_note));

    this->_editIcon = Gdk::Pixbuf::create_from_file("./resource/image/16px/edit-button.png");
    this->_deleteIcon = Gdk::Pixbuf::create_from_file("./resource/image/16px/remove-file.png");

	m_refGlade->get_widget("StatusBar", this->statusBar);
	m_refGlade->get_widget("ScrollWindow", this->_scrolledWindow);

    Gtk::Window::set_resizable(false);
	Gtk::Window::signal_show().connect(sigc::mem_fun(this, &Window::signal_show));

	Gtk::Window::signal_delete_event().connect(sigc::mem_fun(this, &Window::signal_hide));
}

void Window::signal_show() noexcept
{
	this->_dispatcher->handler()->event(this->get_name(), Event::SHOW);
}

bool Window::signal_hide(GdkEventAny* event) noexcept
{
	this->_dispatcher->handler()->event(this->get_name(), Event::HIDE);
	return true;
}

void Window::app(Glib::RefPtr<Gtk::Application> app) noexcept
{
    this->_app = app;
}

void Window::set_size(int width, int height) noexcept
{
    Gtk::Window::set_default_size(width, height);
}

void Window::set_style(std::string_view path) noexcept
{
    auto cssProvider = Gtk::CssProvider::create();
    cssProvider->load_from_path(path.data());

	Gtk::Window::get_style_context()->add_provider_for_screen(Gtk::Window::get_screen(), cssProvider, GTK_STYLE_PROVIDER_PRIORITY_USER);
}

auto find_widget(const std::vector<Gtk::Widget*>& array, std::string_view id)
{
	return std::find_if(std::begin(array), std::end(array), [&id] (auto var){return var->get_name() == id.data();});
}

void Window::show()
{
    this->_app->add_window(*this);
	Gtk::Window::show();
}

void Window::hide() 
{
	clear_list(this->_listBox);
    this->_app->remove_window(*this);
	Gtk::Window::hide();
}

void Window::clear() noexcept
{
	clear_list(this->_listBox);
}

void Window::modal(bool flag) noexcept
{
	Gtk::Window::set_modal(flag);
}

void Window::set_title(std::string_view title) noexcept 
{
	Gtk::Window::set_title(title.data());
}

std::string Window::get_title() const noexcept 
{
	return Gtk::Window::get_title();
}

void Window::set_status_message(std::string_view status) noexcept
{
	if(this->statusBar) this->statusBar->set_text(status.data());
}

void Window::set_name(std::string_view name) noexcept 
{
	Gtk::Window::set_name(name.data());
}

std::string Window::get_name() const noexcept 
{
	return Gtk::Window::get_name();
}

void Window::set_dispatcher(std::shared_ptr<IDispatcher> dispatcher) noexcept
{
	this->_dispatcher = std::move(dispatcher);
}

void Window::button_add_new_item() noexcept
{
	this->_dispatcher->handler()->event(this->get_name(), Event::INSERT);
}

void Window::button_save_note() noexcept
{
    this->_dispatcher->handler()->event(this->get_name(), Event::SAVE);
}

void Window::button_edit_click(Gtk::Button* button, Gtk::ListBoxRow* row) noexcept
{
    if(!row) return;

    this->_dispatcher->handler()->event(this->get_name(), Event::OPEN, row->get_index(), WindowType::EDIT);
}

void Window::button_delete_click(Gtk::Button* button, Gtk::ListBoxRow* row) noexcept
{
    if(!row) return;

	this->_dispatcher->handler()->event(this->get_name(), Event::DELETE, row->get_index());

	remove_row(this->_listBox, row);
}

void Window::toggle_check(Gtk::Button* button, Gtk::ListBoxRow* row) noexcept
{
    if(!row) return;

    std::vector<Gtk::Widget*>  widgets = row->get_children();

    Gtk::Box* box = dynamic_cast<Gtk::Box*>(widgets.at(0));

    if(!box) return;

    widgets = box->get_children();

    auto value = find_widget(widgets, "NotesActive");
    if(value != std::end(widgets))
    {
        Gtk::CheckButton* active = static_cast<Gtk::CheckButton*>(*value);
        if(active->get_active())
            this->_dispatcher->handler()->event(this->get_name(), Event::DEACTIVATE, row->get_index());
        else
            this->_dispatcher->handler()->event(this->get_name(), Event::ACTIVATE, row->get_index());
    }
}

Gtk::ListBoxRow* Window::create_new_row(const Data& value) noexcept
{
	auto rowCount = this->_listBox->get_children().size();
	auto newRowIndex = rowCount + 1;

	Gtk::Label* index = Gtk::manage(new Gtk::Label);
	index->set_text(std::to_string(newRowIndex) + std::string("."));

    Gtk::ListBoxRow* row = Gtk::manage(new Gtk::ListBoxRow());
    row->property_activatable() = false;
    row->property_selectable() = false;

	if(0 != (newRowIndex %2 )) row->get_style_context()->add_class("odd");

    Gtk::CheckButton* check = Gtk::manage(new Gtk::CheckButton);
    check->property_active() = value.status;
    check->set_name("NotesActive");
    auto toggle_handle = [this, row=row, check=check](){this->toggle_check(check, row);};
    check->signal_pressed().connect(toggle_handle);

    Gtk::Label* label = Gtk::manage(new Gtk::Label);
    label->set_text(value.title);
    label->set_name("NotesLabel");
    label->set_halign(Gtk::Align::ALIGN_FILL);

    Gtk::Box* box = Gtk::manage(new Gtk::Box());
    box->pack_start(*index, false, false, 12);
    box->pack_start(*check, false, false, 12);
    box->pack_start(*label, true, true, 10);

    Gtk::Grid* buttons = this->create_tool_buttons(row);
	buttons->set_column_spacing(5);

    box->pack_start(*buttons, false, false, 5);

    row->add(*box);

    return row;
}

void Window::show_data(const Data& value) noexcept
{
    this->_listBox->append(*this->create_new_row(value));
    this->_listBox->show_all_children();
	auto vAdjustment = this->_scrolledWindow->get_vadjustment();
	vAdjustment->set_value(vAdjustment->get_upper());
}

Gtk::Grid* Window::create_tool_buttons(Gtk::ListBoxRow* row)
{
    Gtk::Image* editImage = Gtk::manage(new Gtk::Image(this->_editIcon));
    Gtk::Image* deleteImage = Gtk::manage(new Gtk::Image(this->_deleteIcon));

    Gtk::Grid* grid = Gtk::manage(new Gtk::Grid);

    Gtk::Button* buttonEdit = Gtk::manage(new Gtk::Button());
    buttonEdit->set_image_position(Gtk::POS_LEFT);
    buttonEdit->set_name("NotesButtonEdit");
    buttonEdit->set_image(*editImage);
    auto button_edit_handle = [this, row=row, buttonEdit=buttonEdit](){this->button_edit_click(buttonEdit, row);};
    buttonEdit->signal_pressed().connect(button_edit_handle);

    Gtk::Button* buttonDelete = Gtk::manage(new Gtk::Button());
    buttonDelete->set_image_position(Gtk::POS_LEFT);
    buttonDelete->set_name("NotesButtonDelete");
    buttonDelete->set_image(*deleteImage);
    auto button_delete_handle = [this, row=row, buttonDelete=buttonDelete](){this->button_delete_click(buttonDelete, row);};
    buttonDelete->signal_pressed().connect(button_delete_handle);

    grid->add(*buttonEdit);
    grid->add(*buttonDelete);

    return grid;
}
