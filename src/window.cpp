#include "../header/window.h"
#include "../data.h"

#include <algorithm>

#include <gtkmm-3.0/gtkmm/box.h>
#include <gtkmm-3.0/gtkmm/button.h>
#include <gtkmm-3.0/gtkmm/entry.h>
#include <gtkmm-3.0/gtkmm/image.h>
#include <gtkmm-3.0/gtkmm/checkbutton.h>

Window::Window(BaseObjectType* cobject,
	const Glib::RefPtr<Gtk::Builder>& m_refGlade) : Gtk::Window(cobject)
{
    m_refGlade->get_widget("listBox", this->_listBox);
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

void Window::selected_row(Gtk::ListBoxRow* row) noexcept
{
    if(!row) return;

	Gtk::Box* box;
	Gtk::CheckButton* active;
	Gtk::Entry* text;
	Gtk::Button* editButton;
	Gtk::Button* deleteButton;

    std::vector<Gtk::Widget*>  widgets = row->get_children();

    box = dynamic_cast<Gtk::Box*>(widgets.at(0));
    if(!box) return;
    widgets = box->get_children();

    auto value = find_widget(widgets, "NotesActive");
    if(value != std::end(widgets))
    {
        active = static_cast<Gtk::CheckButton*>(*value);
    }

    value = find_widget(widgets, "NotesLabel");
    if(value != std::end(widgets))
    {
        text = static_cast<Gtk::Entry*>(*value);
    }

    value = find_widget(widgets, "NotesButtonEdit");
    if(value != std::end(widgets))
    {
        editButton = static_cast<Gtk::Button*>(*value);
    }

    value = find_widget(widgets, "NotesButtonDelete");
    if(value != std::end(widgets))
    {
        deleteButton = static_cast<Gtk::Button*>(*value);
    }
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

void Window::add_data(std::vector<struct data> value) noexcept
{
    for(const auto& var : value)
        this->add_data(var);
}

void Window::button_edit_click(Gtk::Button* button, Gtk::ListBoxRow* row) noexcept
{
    if(!row) return;

    std::vector<Gtk::Widget*>  widgets = row->get_children();

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

            this->_dispatcher->handler()->event(this->get_name(), EventType::CHANGE, box->get_name().data(), text->get_text().data());
        }
    }
}

void Window::button_delete_click(Gtk::Button* button, Gtk::ListBoxRow* row) noexcept
{
    if(!row) return;

    std::vector<Gtk::Widget*>  widgets = row->get_children();

    Gtk::Box* box = dynamic_cast<Gtk::Box*>(widgets.at(0));

    if(box)
        this->_dispatcher->handler()->event(this->get_name(), EventType::DELETE, box->get_name().data());
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
            this->_dispatcher->handler()->event(this->get_name(), EventType::DEACTIVATE, box->get_name().data());
        else
            this->_dispatcher->handler()->event(this->get_name(), EventType::ACTIVATE, box->get_name().data());
    }
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
	box->set_name(value.index);
	box->pack_start(*check, false, false, 12);
	box->pack_start(*label, true, true, 10);
	box->pack_start(*buttonEdit, false, false, 5);
	box->pack_start(*buttonDelete, false, false);

    row->add(*box);

    this->_listBox->append(*row);
    this->_listBox->show_all_children();
    editImage->show();
    deleteImage->show();
}

std::vector<struct data> Window::get_data() const noexcept 
{
    return {};
}
