#include "../header/grid.h"
#include "atkmm/object.h"
#include "gtkmm/box.h"
#include "gtkmm/enums.h"
#include "gtkmm/flowbox.h"
#include "gtkmm/label.h"
#include "../data.h"
#include "pangomm/layout.h"
#include "sigc++/functors/mem_fun.h"
#include <iostream>

GridRow::GridRow():Gtk::FlowBox(),_handler(nullptr)
{
	Gtk::FlowBox::signal_child_activated().connect(sigc::mem_fun(this, &GridRow::signal_selected));
	Gtk::FlowBox::set_selection_mode(Gtk::SelectionMode::SELECTION_SINGLE);
}

Gtk::FlowBoxChild* GridRow::create_item(const Block& value) noexcept
{
	Gtk::FlowBoxChild* child = Gtk::manage(new Gtk::FlowBoxChild());
	Gtk::Box* box = this->create_box(value);
	child->add(*box);

	return child;
}

Gtk::Box* GridRow::create_box(const Block &value) noexcept
{
	unsigned int list_items_count = this->get_rows_count();
	auto rowIndex = list_items_count + 1;

	Gtk::Label* newItemRowIndexLabel = Gtk::manage(new Gtk::Label);
	newItemRowIndexLabel->set_text(std::to_string(rowIndex) + std::string("."));

	Gtk::Label* newItemRowTitleLabel = Gtk::manage(new Gtk::Label);
	newItemRowTitleLabel->set_text(value.get_header());
	newItemRowTitleLabel->set_alignment(Gtk::ALIGN_CENTER, Gtk::ALIGN_CENTER);
	newItemRowTitleLabel->set_name("NotesLabel");
	newItemRowTitleLabel->set_halign(Gtk::Align::ALIGN_FILL);
	newItemRowTitleLabel->set_line_wrap_mode(Pango::WrapMode::WRAP_CHAR);
	newItemRowTitleLabel->set_has_tooltip();
	newItemRowTitleLabel->set_tooltip_text(value.get_note());
	
	Gtk::CheckButton* newRowItemCheckButton = this->create_row_check(value.is_active());
	auto toggle_handle = [this, button=newRowItemCheckButton](){
		this->signal_activate_button_click(button);};
	newRowItemCheckButton->signal_pressed().connect(toggle_handle);
	newRowItemCheckButton->set_halign(Gtk::ALIGN_CENTER);

	Gtk::Box* box = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));
	box->pack_start(*newItemRowIndexLabel, false, true, 12);
	box->pack_start(*newRowItemCheckButton, false, false, 12);
	box->pack_start(*newItemRowTitleLabel, false, false, 10);

	return box;
}

Gtk::CheckButton* GridRow::create_row_check(bool status) noexcept
{
    Gtk::CheckButton* checkButton = Gtk::manage(new Gtk::CheckButton);
    checkButton->property_active() = status;
    checkButton->set_name("NotesActive");

	return checkButton;
}

unsigned int GridRow::get_rows_count() const noexcept
{
	return Gtk::FlowBox::get_children().size();
}

void GridRow::signal_activate_button_click(Gtk::CheckButton *button) noexcept
{
	Gtk::FlowBoxChild* selected = (Gtk::FlowBoxChild*)(button->get_parent()->get_parent());
	if(selected != nullptr)
	{
		if(button->get_active())
		{
			this->_handler->event(ComponentEventTypes::DEACTIVATE, selected->get_index());
		}
		else
		{
			this->_handler->event(ComponentEventTypes::ACTIVATE, selected->get_index());
		}
	}
}

void GridRow::signal_selected(Gtk::FlowBoxChild* row) noexcept
{
	if(nullptr == row) return;
	int currentIndex = row->get_index();
	this->_handler->event(ComponentEventTypes::SELECT, currentIndex);
}

void GridRow::set_event_handler(IComponentEvent* handler) noexcept
{
	this->_handler = handler;
}

int GridRow::get_selected_item() const noexcept
{
	auto val = Gtk::FlowBox::get_selected_children();
	if(!val.empty())
	{
		auto begin = val.begin();
		return (*begin)->get_index();
	}
	
	return -1;
}

void GridRow::add(const Block& value) noexcept
{
	Gtk::FlowBox::add(*this->create_item(value));
	Gtk::FlowBox::show_all_children();
}

void GridRow::clear() noexcept
{
	auto rows = Gtk::FlowBox::get_children();
	for(auto row : rows) Gtk::FlowBox::remove(*row);
}
