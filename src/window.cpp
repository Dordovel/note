#include "../header/window.h"

#include "../data.h"
#include "../window_types.hpp"
#include "gtkmm/enums.h"
#include "gtkmm/listboxrow.h"

#include <algorithm>
#include <iostream>

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
	void remove_row_from_listbox(Gtk::ListBox* list, Gtk::ListBoxRow* row) noexcept
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
	this->_addNewItemButton->signal_clicked().connect(sigc::mem_fun(this, &Window::signal_add_button_click));

	m_refGlade->get_widget("SaveNoteButton", this->_saveNoteButton);
	this->_saveNoteButton->signal_clicked().connect(sigc::mem_fun(this, &Window::signal_save_button_click));

    this->_editIcon = Gdk::Pixbuf::create_from_file("./resource/image/16px/edit-button.png");
    this->_deleteIcon = Gdk::Pixbuf::create_from_file("./resource/image/16px/remove-file.png");

	m_refGlade->get_widget("StatusBar", this->statusBar);
	m_refGlade->get_widget("ScrollWindow", this->_scrolledWindow);

    Gtk::Window::set_resizable(false);
	Gtk::Window::signal_show().connect(sigc::mem_fun(this, &Window::signal_show_window));

	Gtk::Window::signal_delete_event().connect(sigc::mem_fun(this, &Window::signal_hide_window));
}

void Window::signal_show_window() noexcept
{
	this->_eventDispatcher->handler()->event(this->id(), Event::SHOW);
}

bool Window::signal_hide_window(GdkEventAny* event) noexcept
{
	this->_eventDispatcher->handler()->event(this->id(), Event::HIDE);
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

auto find_widget(const std::vector<Gtk::Widget*>& array, std::string_view id)
{
	return std::find_if(std::begin(array), std::end(array), [&id] (auto var){return var->get_name() == id.data();});
}

void Window::open()
{
    this->_app->add_window(*this);
	Gtk::Window::show();
}

void Window::close()
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

void Window::set_event_dispatcher(std::shared_ptr<IDispatcher> dispather) noexcept
{
	this->_eventDispatcher = std::move(dispather);
}

void Window::signal_add_button_click() noexcept
{
	this->_eventDispatcher->handler()->event(this->id(), Event::INSERT);
}

void Window::signal_save_button_click() noexcept
{
    this->_eventDispatcher->handler()->event(this->id(), Event::SAVE);
}

void Window::signal_edit_button_click(std::size_t rowIndex) noexcept
{
    this->_eventDispatcher->handler()->event(this->id(), Event::OPEN, rowIndex, WindowType::EDIT);
}

void Window::signal_delete_button_click(std::size_t rowIndex) noexcept
{
	this->_eventDispatcher->handler()->event(this->id(), Event::DELETE, rowIndex);
	auto removedRow = this->_listBox->get_row_at_index(rowIndex);
    remove_row_from_listbox(this->_listBox, removedRow);
}

void Window::signal_activate_button_click(Gtk::CheckButton* button) noexcept
{
	Gtk::ListBoxRow* checkButtonRow = (Gtk::ListBoxRow*)(button->get_parent()->get_parent());
	if(button->get_active())
	{
		checkButtonRow->get_style_context()->remove_class("activeListRow");
		this->_eventDispatcher->handler()->event(this->id(), Event::DEACTIVATE, checkButtonRow->get_index());
	}
	else
	{
		checkButtonRow->get_style_context()->add_class("activeListRow");
		this->_eventDispatcher->handler()->event(this->id(), Event::ACTIVATE, checkButtonRow->get_index());
	}
}

Gtk::CheckButton* create_row_check(bool status)
{
    Gtk::CheckButton* checkButton = Gtk::manage(new Gtk::CheckButton);
    checkButton->property_active() = status;
    checkButton->set_name("NotesActive");

	return checkButton;
}

Gtk::ListBoxRow* Window::create_row(const Data& value) noexcept
{
	auto rowCount = this->_listBox->get_children().size();
	auto newRowIndex = rowCount + 1;

	Gtk::Label* newItemRowIndexLabel = Gtk::manage(new Gtk::Label);
	newItemRowIndexLabel->set_text(std::to_string(newRowIndex) + std::string("."));

    Gtk::Label* newItemRowTitleLabel = Gtk::manage(new Gtk::Label);
    newItemRowTitleLabel->set_text(value.title);
	newItemRowTitleLabel->set_alignment(Gtk::ALIGN_START, Gtk::ALIGN_CENTER);
    newItemRowTitleLabel->set_name("NotesLabel");
    newItemRowTitleLabel->set_halign(Gtk::Align::ALIGN_FILL);
    newItemRowTitleLabel->set_has_tooltip();
    newItemRowTitleLabel->set_tooltip_text(value.note);
	
	Gtk::CheckButton* newRowItemCheckButton = create_row_check(value.status);
    auto toggle_handle = [this, button=newRowItemCheckButton](){
		this->signal_activate_button_click(button);};
    newRowItemCheckButton->signal_pressed().connect(toggle_handle);

    Gtk::Box* box = Gtk::manage(new Gtk::Box());
    box->pack_start(*newItemRowIndexLabel, false, false, 12);
    box->pack_start(*newRowItemCheckButton, false, false, 12);
    box->pack_start(*newItemRowTitleLabel, true, true, 10);

    Gtk::ListBoxRow* row = Gtk::manage(new Gtk::ListBoxRow());
    row->property_activatable() = false;
    row->property_selectable() = false;
    auto rowStyleContext = row->get_style_context();
    if(0 != (newRowIndex %2)) rowStyleContext->add_class("oddListRow");
    else rowStyleContext->add_class("evenListRow");
    if(value.status) rowStyleContext->add_class("activeListRow");

    Gtk::Grid* toolsButtonGrid = this->tool_buttons(rowCount);
	toolsButtonGrid->set_column_spacing(5);

    box->pack_start(*toolsButtonGrid, false, false, 5);

    row->add(*box);

    return row;
}

void Window::print(const Data& value) noexcept
{
    this->_listBox->append(*this->create_row(value));
    this->_listBox->show_all_children();
	auto vAdjustment = this->_scrolledWindow->get_vadjustment();
	vAdjustment->set_value(vAdjustment->get_upper());
}

Gtk::Grid* Window::tool_buttons(size_t rowIndex)
{
    Gtk::Image* editImage = Gtk::manage(new Gtk::Image(this->_editIcon));
    Gtk::Image* deleteImage = Gtk::manage(new Gtk::Image(this->_deleteIcon));

    Gtk::Grid* grid = Gtk::manage(new Gtk::Grid);

    Gtk::Button* buttonEdit = Gtk::manage(new Gtk::Button());
    buttonEdit->set_image_position(Gtk::POS_LEFT);
    buttonEdit->set_name("NotesButtonEdit");
    buttonEdit->set_image(*editImage);
    auto button_edit_handle = [this, index = rowIndex](){
		this->signal_edit_button_click(index);};
    buttonEdit->signal_pressed().connect(button_edit_handle);

    Gtk::Button* buttonDelete = Gtk::manage(new Gtk::Button());
    buttonDelete->set_image_position(Gtk::POS_LEFT);
    buttonDelete->set_name("NotesButtonDelete");
    buttonDelete->set_image(*deleteImage);
    auto button_delete_handle = [this, index = rowIndex](){
		this->signal_delete_button_click(index);};
    buttonDelete->signal_pressed().connect(button_delete_handle);

    grid->add(*buttonEdit);
    grid->add(*buttonDelete);

    return grid;
}
