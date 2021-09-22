#include "../header/main_window.h"
#include "../window_types.hpp"

#include <gtkmm-3.0/gtkmm/grid.h>
#include <gtkmm-3.0/gtkmm/button.h>
#include <gtkmm-3.0/gtkmm/image.h>

#include <iostream>

MainWindow::MainWindow(BaseObjectType* cobject, 
			const Glib::RefPtr<Gtk::Builder>& m_refGlade):Window(cobject, m_refGlade)
{
    this->_openIcon = Gdk::Pixbuf::create_from_file("./resource/image/16px/settings-button.png");
}

Gtk::Grid* MainWindow::create_tool_buttons(size_t rowIndex)
{
    Gtk::Grid* grid = Window::create_tool_buttons(rowIndex);

    Gtk::Image* openImage = Gtk::manage(new Gtk::Image(this->_openIcon));

    Gtk::Button* buttonOpen = Gtk::manage(new Gtk::Button());
    buttonOpen->set_image_position(Gtk::POS_LEFT);
    buttonOpen->set_name("NotesButtonOpen");
    buttonOpen->set_image(*openImage);
    auto button_open_handle = [this, index=rowIndex](){this->button_open_click(index);};
    buttonOpen->signal_pressed().connect(button_open_handle);

    grid->insert_column(0);
    grid->attach(*buttonOpen, 0, 0);

    return grid;
}

void MainWindow::button_open_click(std::size_t rowIndex) noexcept
{
    this->_dispatcher->handler()->event(this->get_window_id(), Event::OPEN, rowIndex, WindowType::LIST);
}

void MainWindow::set_event_dispatcher(std::shared_ptr <IDispatcher> dispatcher) noexcept
{
    Window::set_event_dispatcher(dispatcher);
    this->_dispatcher = std::move(dispatcher);
}
