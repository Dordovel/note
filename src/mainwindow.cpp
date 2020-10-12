#include "../header/mainwindow.h"

#include <gtkmm-3.0/gtkmm/grid.h>
#include <gtkmm-3.0/gtkmm/button.h>
#include <gtkmm-3.0/gtkmm/image.h>

#include <iostream>

MainWindow::MainWindow(BaseObjectType* cobject, 
			const Glib::RefPtr<Gtk::Builder>& m_refGlade):Window(cobject, m_refGlade)
{
    this->_openIcon = Gdk::Pixbuf::create_from_file("../resource/image/16px/settings-button.png");
}

Gtk::Grid* MainWindow::create_tool_buttons(Gtk::ListBoxRow* row)
{
    Gtk::Grid* grid = Window::create_tool_buttons(row);

    Gtk::Image* openImage = Gtk::manage(new Gtk::Image(this->_openIcon));

    Gtk::Button* buttonOpen = Gtk::manage(new Gtk::Button());
    buttonOpen->set_image_position(Gtk::POS_LEFT);
    buttonOpen->set_name("NotesButtonOpen");
    buttonOpen->set_image(*openImage);
    auto button_open_handle = [this, row=row, buttonOpen=buttonOpen](){this->button_open_click(buttonOpen, row);};
    buttonOpen->signal_pressed().connect(button_open_handle);

    grid->insert_column(0);
    grid->attach(*buttonOpen, 0, 0);

    return grid;
}

void MainWindow::button_open_click(Gtk::Button* button, Gtk::ListBoxRow* row) noexcept
{
    this->_dispatcher->handler()->event(this->get_name(), EventType::OPEN, row->get_index(), Window::get_sub_window());
}

void MainWindow::set_dispatcher(const std::shared_ptr <IDispatcher>& dispather) noexcept
{
    Window::set_dispatcher(dispather);
    this->_dispatcher = dispather;
}
