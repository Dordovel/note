#include "../header/window.h"

#include "../data.h"

#include <algorithm>

#include <gtkmm-3.0/gtkmm/box.h>
#include <gtkmm-3.0/gtkmm/button.h>
#include <gtkmm-3.0/gtkmm/entry.h>
#include <gtkmm-3.0/gtkmm/image.h>
#include <gtkmm-3.0/gtkmm/checkbutton.h>

#include <iostream>

Window::Window(BaseObjectType* cobject,
	const Glib::RefPtr<Gtk::Builder>& m_refGlade) : Gtk::Window(cobject)
{
    m_refGlade->get_widget("listBox", this->_listBox);
	m_refGlade->get_widget("addNewItemButton", this->_addNewItemButton);
	this->_addNewItemButton->signal_clicked().connect(sigc::mem_fun(this, &Window::button_add_new_item));

	m_refGlade->get_widget("saveNoteButton", this->_saveNoteButton);
	this->_saveNoteButton->signal_clicked().connect(sigc::mem_fun(this, &Window::button_save_note));

	m_refGlade->get_widget("deleteNoteButton", this->_deleteNoteButton);
	this->_deleteNoteButton->signal_clicked().connect(sigc::mem_fun(this, &Window::button_delete_note));

	
    this->_editIcon = Gdk::Pixbuf::create_from_file("./resource/image/16px/edit-button.png");
    this->_deleteIcon = Gdk::Pixbuf::create_from_file("./resource/image/16px/remove-file.png");

    Gtk::Window::set_resizable(false);
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
	Gtk::Window::on_show();
}

void Window::hide() 
{
    this->_app->remove_window(*this);
	Gtk::Window::hide();
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
	const auto children = this->_listBox->get_children();
	int index = 0;

	if(!children.empty())
	{
		Gtk::ListBoxRow* lastRow = this->_listBox->get_row_at_index(children.size() - 1);
		Gtk::Box* box = dynamic_cast<Gtk::Box*> (lastRow->get_children().at(0));

		if(!box) return;

		index = std::stoi(box->get_name()) + 1;
	}

	this->add_data({index, "", true});
}

void Window::button_save_note() noexcept
{
    this->_dispatcher->handler()->event(this->get_name(), EventType::SAVE);
}

void Window::button_delete_note() noexcept
{
    this->_dispatcher->handler()->event(this->get_name(), EventType::DELETE);
}

void Window::button_edit_click(Gtk::Button* button, Gtk::ListBoxRow* row) noexcept
{
    if(!row) return;

    std::vector<Gtk::Widget*> widgets = row->get_children();

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
        }
    }
}

void Window::button_delete_click(Gtk::Button* button, Gtk::ListBoxRow* row) noexcept
{
    if(!row) return;

	this->_listBox->remove(*row);
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

			auto style = text->get_style_context();

			if(active->get_active())
			{
				style->remove_class("entryActive");
				style->add_class("entryDeactive");
			}
			else
			{
				style->remove_class("entryDeactive");
				style->add_class("entryActive");
			}
		}
    }
}

void Window::add_data(std::vector<struct data> value) noexcept
{
    for(const auto& var : value)
        this->add_data(var);
}

void Window::add_data(struct data value) noexcept
{
    Gtk::Image* editImage = Gtk::manage(new Gtk::Image(this->_editIcon));
    Gtk::Image* deleteImage = Gtk::manage(new Gtk::Image(this->_deleteIcon));

    Gtk::ListBoxRow* row = Gtk::manage(new Gtk::ListBoxRow());
    row->property_activatable() = false;
    row->property_selectable() = false;

    Gtk::CheckButton* check = Gtk::manage(new Gtk::CheckButton);
    check->property_active() = value.status;
	check->set_name("NotesActive");

    Gtk::Entry* label = Gtk::manage(new Gtk::Entry);
    label->set_text(value.text);
	label->set_name("NotesLabel");
	label->property_editable() = false;
	label->property_can_focus() = false;

    Gtk::Button* buttonEdit = Gtk::manage(new Gtk::Button());
    buttonEdit->set_image_position(Gtk::POS_LEFT);
	buttonEdit->set_name("NotesButtonEdit");
	buttonEdit->set_image(*editImage);

    Gtk::Button* buttonDelete = Gtk::manage(new Gtk::Button());
    buttonDelete->set_image_position(Gtk::POS_LEFT);
	buttonDelete->set_name("NotesButtonDelete");
    buttonDelete->set_image(*deleteImage);

    auto toggle_handle = [this, row=row, check=check](){this->toggle_check(check, row);};
    auto button_edit_handle = [this, row=row, buttonEdit=buttonEdit](){this->button_edit_click(buttonEdit, row);};
    auto button_delete_handle = [this, row=row, buttonDelete=buttonDelete](){this->button_delete_click(buttonDelete, row);};

    check->signal_pressed().connect(toggle_handle);
    buttonEdit->signal_pressed().connect(button_edit_handle);
    buttonDelete->signal_pressed().connect(button_delete_handle);

    Gtk::Box* box = Gtk::manage(new Gtk::Box());
	box->set_name(std::to_string(value.index));
	box->pack_start(*check, false, false, 12);
	box->pack_start(*label, true, true, 10);
	box->pack_start(*buttonEdit, false, false, 5);
	box->pack_start(*buttonDelete, false, false);

    row->add(*box);

    this->_listBox->append(*row);
    this->_listBox->show_all_children();
}

std::vector<struct data> Window::get_data() const noexcept 
{
	std::vector<Gtk::Widget*> rows = this->_listBox->get_children();

	std::vector<struct data> list;
	list.reserve(rows.size());

	struct data buffer;

	Gtk::ListBoxRow* row;
	std::vector<Gtk::Widget*> elements;

	Gtk::Box* box;

	for(const auto& value : rows)
	{
		row = static_cast<Gtk::ListBoxRow*>(value);
		box = dynamic_cast<Gtk::Box*>(row->get_children().at(0));

		if(!box) continue;

		buffer.index = std::stoi(box->get_name());

		elements = box->get_children();

		auto element = find_widget(elements, "NotesActive");
		if(element != std::end(elements))
		{
			Gtk::CheckButton* active = static_cast<Gtk::CheckButton*>(*element);

			buffer.status = !active->get_active();
		}

		element = find_widget(elements, "NotesLabel");
		if(element != std::end(elements))
		{
			Gtk::Entry* text = static_cast<Gtk::Entry*>(*element);
			
			buffer.text = text->get_text();
		}

		list.push_back(buffer);

	}
	
    return list;
}
