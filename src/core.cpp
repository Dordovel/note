#include <iostream>
#include "../header/core.h"
#include "../window_types.hpp"
#include <algorithm>


void Core::update_window(Core::_window_& window) noexcept
{
    auto weakPointer = window._window;
    if(auto windowPointer = weakPointer.lock(); windowPointer)
	{
		windowPointer->clear();
		auto page = window._page;
		auto lineCount = page->get_line_count();
		for ( int i = 0; i < lineCount; ++i )
		{
			windowPointer->print(page->get_line_by_id(i));
		}
	}
}

void Core::register_manager(std::unique_ptr<IWindowRegisterGet> manager) noexcept
{
	this->_manager = std::move(manager);
}

void Core::register_page_factory(std::unique_ptr<PageFactory> factory)
{
	this->_pageFactory = std::move(factory);
}

void Core::register_page_saver(std::unique_ptr<PageSaver> saver)
{
	this->_pageSaver = std::move(saver);
}

void Core::event(std::string_view id, CoreEventTypes type, std::size_t index) noexcept
{
	Core::_window_& window = this->_windows.top();
	Block& block = window._page->get_line_by_id(index);

    switch(type)
    {
		case CoreEventTypes::ACTIVATE:
		{
			block.set_active(true);
		}
		break;

        case CoreEventTypes::DEACTIVATE:
		{
			block.set_active(false);
		}
		break;

		case CoreEventTypes::DELETE:
		{
			window._page->delete_line_by_id(index);
			this->update_window(window);
		}
		break;

        default: break;
    }
}

void Core::event(std::string_view id, CoreEventTypes type) noexcept 
{
	if(this->_windows.empty())
	{
		auto fd = this->_manager->window(id);
		Core::_window_ newWindow;
		newWindow._window = fd;
		int pageNumber = this->_windows.empty() ? 0 : this->_windows.top().level;
		newWindow._page = this->_pageFactory->make_page(pageNumber);
		this->_windows.push(newWindow);
	}

	Core::_window_& window = this->_windows.top();
	auto weakPointer = window._window;
	auto windowPointer = weakPointer.lock();
	if(!windowPointer) return;

	switch(type)
	{
        case CoreEventTypes::SAVE:
        {
			this->_pageSaver->save_page(window._page);
			window._page->reset_blocks();
        }
        break;

        case CoreEventTypes::HIDE:
        {
			if(window._blockReferer.empty())
			{
				if(window._page->is_change())
				{
					windowPointer->set_info_message("Changes not saved");
					break;
				}
			}
			windowPointer->close();
			this->_windows.pop();
        }
        break;

		case CoreEventTypes::SHOW:
		{
			window._windowId = id;
			this->update_window(window);
		}
		break;

		case CoreEventTypes::UPDATE:
		{
			this->update_window(window);
		}
		break;

		case CoreEventTypes::INSERT:
		{
			Block block = window._page->insert_line();
			windowPointer->print(block);
		}
		break;

		default: break;
	}
}

void Core::event(std::string_view id, CoreEventTypes type, struct Block value) noexcept
{
    switch(type)
    {
		case CoreEventTypes::CHANGE:
		{
			Core::_window_ last = this->_windows.top();
			this->_windows.pop();

			Core::_window_& window = this->_windows.top();
			auto referers = last._blockReferer;

			if (!referers.empty())
			{
				for(const auto& referer : referers)
				{
					window._page->replace_line(referer.second, value);
				}
			}
			this->update_window(window);
			this->_windows.push(std::move(last));
		}
		break;

        default: break;
    }
}

void Core::event(std::string_view id, CoreEventTypes event, std::size_t index, WindowType type) noexcept
{
    switch(event)
    {
		case CoreEventTypes::OPEN:
		{
			auto weakPointer = this->_manager->window(type);
			auto windowPointer = weakPointer.lock();
			if(!windowPointer) return;

			Core::_window_& window = this->_windows.top();
			std::shared_ptr<IPage> page = window._page;

			if(type == WindowType::EDIT)
            {
				Block editBlock = page->get_line_by_id(index);
				std::unique_ptr<IPage> newPage = this->_pageFactory->make_page();
				newPage->insert_line(std::move(editBlock));

				Core::_window_ newWindow;
				newWindow._blockReferer.emplace(newPage->get_line_count() - 1, index);
				newWindow._page = std::move(newPage);
				newWindow._window = windowPointer;
				this->_windows.push(newWindow);
			}
			else
			{
				if(page->is_change())
				{
					auto windowPointer = weakPointer.lock();
					windowPointer->set_info_message("Changes not saved");
					return;
				}
			}

			windowPointer->modal(true);
			//windowPointer->set_title(row.block.header);
			windowPointer->open();
		}
		break;

        default: break;
    }
}