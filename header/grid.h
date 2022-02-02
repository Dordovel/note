#ifndef GRID_CPP
#define GRID_CPP

#include "gtkmm/checkbutton.h"
#include "gtkmm/flowbox.h"
#include "gtkmm/box.h"
#include "../interface/component_event.h"
#include "../interface/component.h"

#include "../data.h"
#include "gtkmm/flowboxchild.h"

class GridRow : public Gtk::FlowBox, public IComponent
{
	private:
		Gtk::FlowBoxChild* create_item(const Block& value) noexcept;
		Gtk::Box* create_box(const Block& value) noexcept;
		Gtk::CheckButton* create_row_check(bool status) noexcept;
		unsigned int get_rows_count() const noexcept;

		void signal_activate_button_click(Gtk::CheckButton* button) noexcept;
		void signal_selected(Gtk::FlowBoxChild* row) noexcept;

		IComponentEvent* _handler;

	public:
		GridRow();
		void set_event_handler(IComponentEvent* handler) noexcept;

		int get_selected_item() const noexcept override;
		void add(const Block& value) noexcept override;
		void clear() noexcept override;
};

#endif //GRID_CPP
