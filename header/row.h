#ifndef ROW_CPP
#define ROW_CPP

#include "gtkmm/listbox.h"
#include "gtkmm/listboxrow.h"
#include "gtkmm/checkbutton.h"
#include "gtkmm/box.h"
#include "../interface/component_event.h"
#include "../interface/component.h"

#include "../data.h"

class ListRow : public Gtk::ListBox, public IComponent
{
	private:
		Gtk::ListBoxRow* create_item(const Data& value) noexcept;
		Gtk::Box* create_box(const Data& value) noexcept;
		Gtk::CheckButton* create_row_check(bool status) noexcept;
		unsigned int get_rows_count() const noexcept;

		void signal_activate_button_click(Gtk::CheckButton* button) noexcept;
		void signal_selected(Gtk::ListBoxRow* row) noexcept;

		IComponentEvent* _handler;

	public:
		ListRow();
		void set_event_handler(IComponentEvent* handler) noexcept;

		int get_selected_item() const noexcept override;
		void add(const Data& value) noexcept override;
		void clear() noexcept override;
};

#endif //ROW_CPP
