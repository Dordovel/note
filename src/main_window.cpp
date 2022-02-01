#include "../header/main_window.h"
#include "../window_types.hpp"

#include <gtkmm-3.0/gtkmm/grid.h>
#include <gtkmm-3.0/gtkmm/button.h>
#include <gtkmm-3.0/gtkmm/image.h>

#include <iostream>

MainWindow::MainWindow(BaseObjectType* cobject, 
			const Glib::RefPtr<Gtk::Builder>& m_refGlade):Window(cobject, m_refGlade)
{
	m_refGlade->get_widget("NotesButtonOpen", this->_openButton);
	this->_openButton->signal_clicked().connect(sigc::mem_fun(this, &MainWindow::signal_open_button_click));
}

void MainWindow::signal_open_button_click() noexcept
{
	int selectId = this->_component->get_selected_item();
	if(selectId < 0) return;

	this->_openButton->set_visible(true);

    this->_dispatcher->handler()->event(this->id(), CoreEventTypes::OPEN, selectId, WindowType::LIST);
}

void MainWindow::set_event_dispatcher(std::shared_ptr <ICoreDispatcher> dispatcher) noexcept
{
    Window::set_event_dispatcher(dispatcher);
    this->_dispatcher = std::move(dispatcher);
}
