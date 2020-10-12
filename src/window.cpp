#include "../header/window.h"

#include "../data.h"

#include <algorithm>

#include <gtkmm-3.0/gtkmm/box.h>
#include <gtkmm-3.0/gtkmm/button.h>
#include <gtkmm-3.0/gtkmm/entry.h>
#include <gtkmm-3.0/gtkmm/image.h>
#include <gtkmm-3.0/gtkmm/checkbutton.h>
#include <gtkmm-3.0/gtkmm/grid.h>
#include <iostream>

namespace
{
	void deactivate_entry(Gtk::Entry* entry)
	{
		auto style = entry->get_style_context();

		style->remove_class("entryActive");
		style->add_class("entryDeactive");
	}

	void activate_entry(Gtk::Entry* entry)
	{
		auto style = entry->get_style_context();

		style->remove_class("entryDeactive");
		style->add_class("entryActive");
	}

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
    m_refGlade->get_widget("listBox", this->_listBox);
	m_refGlade->get_widget("addNewItemButton", this->_addNewItemButton);
	this->_addNewItemButton->signal_clicked().connect(sigc::mem_fun(this, &Window::button_add_new_item));

	m_refGlade->get_widget("saveNoteButton", this->_saveNoteButton);
	this->_saveNoteButton->signal_clicked().connect(sigc::mem_fun(this, &Window::button_save_note));

    this->_editIcon = Gdk::Pixbuf::create_from_file("../resource/image/16px/edit-button.png");
    this->_deleteIcon = Gdk::Pixbuf::create_from_file("../resource/image/16px/remove-file.png");

    Gtk::Window::set_resizable(false);
	Gtk::Window::signal_show().connect(sigc::mem_fun(this, &Window::signal_show));

	Gtk::Window::signal_hide().connect(sigc::mem_fun(this, &Window::signal_hide));
}

void Window::set_sub_window(std::string_view id) noexcept
{
	this->_subWindow = id;
}

std::string Window::get_sub_window() noexcept
{
	return this->_subWindow;
}

std::string Window::get_parent() noexcept
{
	return this->_parentWindow;
}

void Window::set_parent(std::string_view id) noexcept
{
	this->_parentWindow = id;
}

void Window::signal_show() noexcept
{
	this->_dispatcher->handler()->event(this->get_name(), EventType::SHOW);
}

void Window::signal_hide() noexcept
{
    this->_dispatcher->handler()->event(this->get_name(), EventType::HIDE);
    clear_list(this->_listBox);
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
    this->_cssEntry = Gtk::CssProvider::create();
    this->_cssEntry->load_from_path(path.data());

    _styleContext = Gtk::StyleContext::create();

    this->_screen = Gtk::Window::get_screen();
    this->_styleContext->add_provider_for_screen(this->_screen, this->_cssEntry, GTK_STYLE_PROVIDER_PRIORITY_USER);
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
    this->_app->remove_window(*this);
	Gtk::Window::hide();
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

void Window::set_name(std::string_view name) noexcept 
{
	Gtk::Window::set_name(name.data());
}

std::string Window::get_name() const noexcept 
{
	return Gtk::Window::get_name();
}

void Window::set_dispatcher(const std::shared_ptr<IDispatcher>& dispather) noexcept
{
	this->_dispatcher = dispather;
}

void Window::button_add_new_item() noexcept
{
	this->_dispatcher->handler()->event(this->get_name(), EventType::INSERT);
}

void Window::button_save_note() noexcept
{
    this->_dispatcher->handler()->event(this->get_name(), EventType::SAVE);
}

void Window::button_edit_click(Gtk::Button* button, Gtk::ListBoxRow* row) noexcept
{
    if(!row) return;

    std::vector<Gtk::Widget*> widgets = row->get_children();
	if(widgets.empty())
		return;

    Gtk::Box* box = dynamic_cast<Gtk::Box*>(widgets.at(0));
    if(!box) return;
    widgets = box->get_children();

    auto value = find_widget(widgets, "NotesLabel");
    if(value != std::end(widgets))
    {
		Gtk::Entry* text = static_cast<Gtk::Entry*>(*value);

		if(!text->get_editable())
        {
            text->property_editable() = true;
            text->property_can_focus() = true;
            text->grab_focus_without_selecting();
            text->set_position(text->get_text_length());
        }
        else
        {
            text->property_editable() = false;
            text->property_can_focus() = false;

			this->_dispatcher->handler()->event(this->get_name(), EventType::CHANGE, row->get_index(), text->get_text().data());
        }
    }
}

void Window::button_delete_click(Gtk::Button* button, Gtk::ListBoxRow* row) noexcept
{
    if(!row) return;

	this->_dispatcher->handler()->event(this->get_name(), EventType::DELETE, row->get_index());

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

		auto value = find_widget(widgets, "NotesLabel");
		if(value != std::end(widgets))
		{
			Gtk::Entry* text = static_cast<Gtk::Entry*>(*value);

			if(active->get_active())
			{
				deactivate_entry(text);
				this->_dispatcher->handler()->event(this->get_name(), EventType::DEACTIVATE, row->get_index());
			}
			else
			{
				activate_entry(text);
				this->_dispatcher->handler()->event(this->get_name(), EventType::ACTIVATE, row->get_index());
			}
		}
    }
}

Gtk::ListBoxRow* Window::create_new_row(const Data& value) noexcept
{
    Gtk::ListBoxRow* row = Gtk::manage(new Gtk::ListBoxRow());
    row->property_activatable() = false;
    row->property_selectable() = false;

    Gtk::CheckButton* check = Gtk::manage(new Gtk::CheckButton);
    check->property_active() = value.status;
    check->set_name("NotesActive");
    auto toggle_handle = [this, row=row, check=check](){this->toggle_check(check, row);};
    check->signal_pressed().connect(toggle_handle);

    Gtk::Entry* label = Gtk::manage(new Gtk::Entry);
    label->set_text(value.text);
    label->set_name("NotesLabel");
    label->property_editable() = false;
    label->property_can_focus() = false;
    if(value.status)
        activate_entry(label);
    else
        deactivate_entry(label);

    Gtk::Box* box = Gtk::manage(new Gtk::Box());
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
