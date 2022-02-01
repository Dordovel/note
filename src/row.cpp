#include "../header/row.h"
#include "gtkmm/box.h"
#include "gtkmm/enums.h"
#include "gtkmm/label.h"
#include "../data.h"
#include "gtkmm/listboxrow.h"
#include "sigc++/functors/mem_fun.h"
#include <iostream>

ListRow::ListRow():Gtk::ListBox(),_handler(nullptr)
{
	Gtk::ListBox::signal_row_selected().connect(sigc::mem_fun(this, &ListRow::signal_selected));
	Gtk::ListBox::set_selection_mode(Gtk::SelectionMode::SELECTION_SINGLE);
}

Gtk::ListBoxRow* ListRow::create_item(const Data& value) noexcept
{
	unsigned int list_items_count = this->get_rows_count();
	auto rowIndex = list_items_count + 1;

	Gtk::ListBoxRow* row = Gtk::manage(new Gtk::ListBoxRow());
	auto rowStyleContext = row->get_style_context();
	if(value.status) rowStyleContext->add_class("activeListRow");

	Gtk::Box* box = this->create_box(value);

	row->add(*box);

	return row;
}

Gtk::Box* ListRow::create_box(const Data &value) noexcept
{
	unsigned int list_items_count = this->get_rows_count();
	auto rowIndex = list_items_count + 1;

	Gtk::Label* newItemRowIndexLabel = Gtk::manage(new Gtk::Label);
	newItemRowIndexLabel->set_text(std::to_string(rowIndex) + std::string("."));

	Gtk::Label* newItemRowTitleLabel = Gtk::manage(new Gtk::Label);
	newItemRowTitleLabel->set_text(value.title);
	newItemRowTitleLabel->set_alignment(Gtk::ALIGN_START, Gtk::ALIGN_CENTER);
	newItemRowTitleLabel->set_name("NotesLabel");
	newItemRowTitleLabel->set_halign(Gtk::Align::ALIGN_FILL);
	newItemRowTitleLabel->set_has_tooltip();
	newItemRowTitleLabel->set_tooltip_text(value.note);
	
	Gtk::CheckButton* newRowItemCheckButton = this->create_row_check(value.status);
	auto toggle_handle = [this, button=newRowItemCheckButton](){
		this->signal_activate_button_click(button);};
	newRowItemCheckButton->signal_pressed().connect(toggle_handle);

	Gtk::Box* box = Gtk::manage(new Gtk::Box());
	box->pack_start(*newItemRowIndexLabel, false, false, 12);
	box->pack_start(*newRowItemCheckButton, false, false, 12);
	box->pack_start(*newItemRowTitleLabel, true, true, 10);

	return box;
}

Gtk::CheckButton* ListRow::create_row_check(bool status) noexcept
{
    Gtk::CheckButton* checkButton = Gtk::manage(new Gtk::CheckButton);
    checkButton->property_active() = status;
    checkButton->set_name("NotesActive");

	return checkButton;
}

unsigned int ListRow::get_rows_count() const noexcept
{
	return Gtk::ListBox::get_children().size();
}

void ListRow::signal_activate_button_click(Gtk::CheckButton *button) noexcept
{
	Gtk::ListBoxRow* selectedRow = (Gtk::ListBoxRow*)(button->get_parent()->get_parent());
	if(button->get_active())
	{
		this->_handler->event(ComponentEventTypes::DEACTIVATE, selectedRow->get_index());
	}
	else
	{
		this->_handler->event(ComponentEventTypes::ACTIVATE, selectedRow->get_index());
	}
}

void ListRow::signal_selected(Gtk::ListBoxRow* row) noexcept
{
	if(nullptr == row) return;
	int currentIndex = row->get_index();
	this->_handler->event(ComponentEventTypes::SELECT, currentIndex);
}

void ListRow::set_event_handler(IComponentEvent* handler) noexcept
{
	this->_handler = handler;
}

int ListRow::get_selected_item() const noexcept
{
	if(Gtk::ListBox::get_selected_row())
		return Gtk::ListBox::get_selected_row()->get_index();
	
	return -1;
}

void ListRow::add(const Data& value) noexcept
{
	Gtk::ListBox::append(*this->create_item(value));
	Gtk::ListBox::show_all_children();
}

void ListRow::clear() noexcept
{
	auto rows = Gtk::ListBox::get_children();
	for(auto row : rows) Gtk::ListBox::remove(*row);
}
