#include <iostream>
#include "../header/core.h"
#include "../window_types.hpp"
#include <algorithm>
#include <optional>

Core::Core(std::shared_ptr<IDatabase> database):_database(std::move(database)) {}

Core::_page_ Core::load_page_from_db(int parent) noexcept
{
	std::vector<std::string> columns {"id", "data", "status", "parent", "note"};
	std::vector<std::string> predicate {"deleted = 0", std::string("parent = ") + std::to_string(parent) };

	std::vector<std::unordered_map<std::string, std::string>> result = this->_database->query_select(this->_table, columns, predicate);

	std::vector<Core::_line_> buffer;
	buffer.reserve(result.size());

	for(const auto& row : result)
	{
		Block _row;
		_row.index = std::stoi(row.at("id"));
		_row.is_active = std::stoi(row.at("status"));
		_row.header = row.at("data");
		_row.text = row.at("note");

		Core::_line_ _buffer;
		_buffer.block = _row;
		_buffer.created = Core::_created_::LOAD;
		_buffer.status = Core::_status_::NONE;
		
		buffer.emplace_back(_buffer);
	}

	return Core::_page_{ parent, "", buffer};
}

Core::_page_* Core::current_page() noexcept
{
	return &this->_pages.top();
}

Core::_line_ Core::create_empty_element() noexcept
{
	int index = 0;

	const Core::_page_* pPage = this->current_page();

	if(!pPage->_lines.empty())
	{
		index = ( pPage->_lines.back().block.index + 1 );
	}

	Core::_line_ _data;
	_data.status = Core::_status_::NONE;
	_data.created = Core::_created_::NEW;
	_data.block.index = index;

	return _data;
}

Core::_page_ Core::save_page(Core::_page_ page) const noexcept
{
	for(auto& row : page._lines)
	{
		if(row.created == Core::_created_::NEW)
		{
			this->_database->query_insert(this->_table,
					{"parent", "data", "is_active", "note"},
					{ std::to_string(page._id), row.block.header, std::to_string(row.block.is_active), row.block.text});

			row.block.index = this->_database->last_insert_id();
		}

		if(row.created == Core::_created_::LOAD)
		{
			switch(row.status)
			{
				case Core::_status_::CHANGE:
				{
					this->_database->query_update(this->_table,
							{"data", "is_active", "note"},
							{ row.block.header, std::to_string(row.block.is_active), row.block.text},
							{"id = " + std::to_string(row.block.index), "parent = " + std::to_string(page._id)});
				}
				break;

				case Core::_status_::DELETE:
				{
					this->_database->query_update(this->_table,
							{"deleted"},
							{"1"},
							{"id = " + std::to_string(row.block.index), "parent = " + std::to_string(page._id)});

					this->_database->query_update(this->_table,
							{"deleted"},
							{"1"},
							{"parent = " + std::to_string(row.block.index)});
				}
				break;

				default: break;
			}
		}

		row.status = Core::_status_::NONE; row.created = Core::_created_::LOAD;
	}

	return page;
}

bool Core::is_change(const Core::_page_& page) const noexcept
{
	auto begin = page._lines.begin();
	auto end = page._lines.end();

	for(auto i = begin; i != end; ++i)
	{
		if(i->created == Core::_created_::LOAD)
		{
			if(i->status != Core::_status_::NONE) return false;
		}
		else if (i->created == Core::_created_::NEW)
		{
			if(i->status != Core::_status_::NONE
					&& i->status != Core::_status_::DELETE)
			{
				return false;
			}
		}
	}
	return true;
}

void Core::update_window(std::string_view window, const Core::_page_& page) noexcept
{
    auto weakPointer = this->_manager->window(window);
    auto windowPointer = weakPointer.lock();
    if(windowPointer) this->update_window(&( *windowPointer ), page);
}

void Core::update_window(IWindow* window, const Core::_page_& page) noexcept
{
	window->clear();

	for(const auto& val : page._lines)
	{
		if(val.status == Core::_status_::DELETE) break;
		window->print(val.block);
	}
}

void Core::register_manager(std::unique_ptr<IWindowRegisterGet> manager) noexcept 
{
	this->_manager = std::move(manager);
}

void Core::event(std::string_view id, CoreEventTypes type, std::size_t index) noexcept
{
	Core::_page_* const pPage = this->current_page();

	if(index >= pPage->_lines.size()) return;

    _line_& row = pPage->_lines.at(index);

    switch(type)
    {
		case CoreEventTypes::ACTIVATE:
		{
			row.status = Core::_status_::CHANGE;
			row.block.is_active = true;
		}
		break;

        case CoreEventTypes::DEACTIVATE:
		{
			row.status = Core::_status_::CHANGE;
			row.block.is_active = false;
		}
		break;

		case CoreEventTypes::DELETE:
		{
			row.status = Core::_status_::DELETE;
			decltype(pPage->_lines)::iterator a(&row), b(&row);
			while((++b) != pPage->_lines.end())
			{
				std::swap(*(a++), *b);
			}
			this->update_window(id, *pPage);
		}
		break;

        default: break;
    }
}

void Core::event(std::string_view id, CoreEventTypes type) noexcept 
{
	auto weakPointer = this->_manager->window(id);
	auto window = weakPointer.lock();
	if(!window) return;

	Core::_page_* pPage = this->current_page();

	switch(type)
	{
        case CoreEventTypes::SAVE:
        {
			Core::_page_ newBuffer = this->save_page(*pPage);
			std::swap(this->_pages.top(), newBuffer);
        }
        break;

        case CoreEventTypes::HIDE:
        {
            if(!this->is_change(*pPage))
            {
				window->set_info_message("Changes not saved");
                break;
            }
            this->_pages.pop();
			window->close();
        }
        break;

		case CoreEventTypes::SHOW:
		{
			if(this->_pages.empty()) this->_pages.push(this->load_page_from_db());
			pPage = this->current_page();
			pPage->window = id;

			this->update_window(window.get(), *pPage);
		}
		break;

		case CoreEventTypes::UPDATE:
		{
			this->update_window(window.get(), *pPage);
		}
		break;

		case CoreEventTypes::INSERT:
		{
			Core::_line_ val = this->create_empty_element();
			pPage->_lines.emplace_back(val);

			window->print(val.block);
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
			this->_pages.pop();
			Core::_page_* last = this->current_page();
			auto begin = last->_lines.begin();
			auto end = last->_lines.end();
			auto row = std::find_if(begin, end, [id = value.index](auto val){ return val.block.index == id; });

		    if( row->block.text != value.text || row->block.header != value.header)
            {
                row->status = Core::_status_::CHANGE;
                row->block = value;

				this->update_window(last->window, *last);
            }

			this->_pages.emplace();
		}
		break;

        default: break;
    }
}

void Core::event(std::string_view id, CoreEventTypes type, std::size_t index, WindowType window) noexcept
{
    Core::_page_* const pPage = this->current_page();
    Core::_line_& row = pPage->_lines.at(index);

    switch(type)
    {
		case CoreEventTypes::OPEN:
		{
			auto weakPointer = this->_manager->window(window);
			auto windowPointer = weakPointer.lock();
			if(!windowPointer) return;

			if(window == WindowType::EDIT)
            {
				Core::_page_ page;
				page.window = pPage->window;
				page._id = pPage->_id;
				page._lines.push_back(row);
                this->_pages.push(page);
            }
			else
			{
				if(!this->is_change(*pPage))
				{
					auto weakPointer = this->_manager->window(pPage->window);
					auto windowPointer = weakPointer.lock();
					windowPointer->set_info_message("Changes not saved");
					return;
				}

                this->_pages.push(this->load_page_from_db(row.block.index));
			}

			windowPointer->modal(true);
			windowPointer->set_title(row.block.header);
			windowPointer->open();
		}
		break;

        default: break;
    }
}
