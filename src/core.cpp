#include <iostream>
#include "../header/core.h"
#include "../window_types.hpp"
#include <algorithm>

namespace 
{
	template<typename Container>
	auto get_element(const Container& container, std::string_view element)
	{
		return std::find_if(std::begin(container), std::end(container), [element] (auto value)
			{return value.first == element;});
	}
};

Core::Core(std::shared_ptr<IDatabase> database):_database(std::move(database)) {}

Core::_buffer_ Core::load_buffer(int parent) noexcept
{
	std::vector<std::string> columns {"id", "data", "status", "parent", "note"};
	std::vector<std::string> predicate {"deleted = 0", std::string("parent = ") + std::to_string(parent) };

	std::vector<std::unordered_map<std::string, std::string>> result = this->_database->query_select(this->_table, columns, predicate);

	std::vector<Core::_data_> buffer;
	buffer.reserve(result.size());

	for(const auto& row : result)
	{
		Data _row;
		_row.index = std::stoi(row.at("id"));
		_row.status = std::stoi(row.at("status"));
		_row.title = row.at("data");
		_row.note = row.at("note");

		Core::_data_ _buffer;
		_buffer.data = _row;
		_buffer.created = Core::_created_::LOAD;
		_buffer.status = Core::_status_::NONE;
		
		buffer.emplace_back(_buffer);
	}

	return Core::_buffer_{parent, "", buffer};
}

Core::_buffer_* Core::current_buffer() noexcept
{
	return &this->_pages.top();
}

Core::_data_ Core::create_empty_element() noexcept { int index = 0;

	const Core::_buffer_* pBuffer = this->current_buffer();

	if(!pBuffer->_data.empty())
	{
		index = ( pBuffer->_data.back().data.index + 1 );
	}

	Core::_data_ _data;
	_data.status = Core::_status_::CHANGE;
	_data.created = Core::_created_::NEW;
	_data.data.index = index;

	return _data;
}

Core::_buffer_ Core::save_buffer(Core::_buffer_ buffer) const noexcept
{
	for(auto& row : buffer._data)
	{
		if(row.created == Core::_created_::NEW)
		{
			this->_database->query_insert(this->_table,
					{"parent", "data", "status", "note"},
					{ std::to_string(buffer._id), row.data.title, std::to_string(row.data.status), row.data.note});

			row.data.index = this->_database->last_insert_id();
		}

		if(row.created == Core::_created_::LOAD)
		{
			switch(row.status)
			{
				case Core::_status_::CHANGE:
				{
					this->_database->query_update(this->_table,
							{"data", "status", "note"},
							{ row.data.title, std::to_string(row.data.status), row.data.note},
							{"id = " + std::to_string(row.data.index), "parent = " + std::to_string(buffer._id)});
				}
				break;

				case Core::_status_::DELETE:
				{
					this->_database->query_update(this->_table,
							{"deleted"},
							{"1"},
							{"id = " + std::to_string(row.data.index), "parent = " + std::to_string(buffer._id)});

					this->_database->query_update(this->_table,
							{"deleted"},
							{"1"},
							{"parent = " + std::to_string(row.data.index)});
				}
				break;

				default: break;
			}
		}

		row.status = Core::_status_::NONE; row.created = Core::_created_::LOAD;
	}

	return buffer;
}

bool Core::buffer_empty(const Core::_buffer_& buffer) const noexcept
{
	auto begin = buffer._data.begin();
	auto end = buffer._data.end();

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

void Core::update_window_buffer(std::string_view window, const Core::_buffer_& buffer) noexcept
{
    auto weakPointer = this->_manager->get_window(window);
    auto windowPointer = weakPointer.lock();
    if(windowPointer) this->update_window_buffer(&(*windowPointer), buffer);
}

void Core::update_window_buffer(IWindow* window, const Core::_buffer_& buffer) noexcept
{
    window->clear();

	for(const auto& val : buffer._data)
		window->show_data(val.data);
}

void Core::register_manager(std::unique_ptr<IWindowRegisterGet> manager) noexcept 
{
	this->_manager = std::move(manager);
}

void Core::event(std::string_view id, Event type, std::size_t index) noexcept
{
	Core::_buffer_* const pBuffer = this->current_buffer();

	if(index >= pBuffer->_data.size()) return;

    _data_& row = pBuffer->_data.at(index);

    switch(type)
    {
		case Event::ACTIVATE:
		{
			if(!row.data.status)
			{
				row.status = Core::_status_::CHANGE;
				row.data.status = true;
			}
		}
		break;

        case Event::DEACTIVATE:
		{
			if(row.data.status)
			{
				row.status = Core::_status_::CHANGE;
				row.data.status = false;
			}
		}
		break;

		case Event::DELETE:
		{
			row.status = Core::_status_::DELETE;
		}
		break;

        default: break;
    }
}

void Core::event(std::string_view id, Event type) noexcept 
{
	auto weakPointer = this->_manager->get_window(id);
	auto window = weakPointer.lock();
	if(!window) return;

	Core::_buffer_* const pBuffer = this->current_buffer();

	switch(type)
	{
        case Event::SAVE:
        {
			Core::_buffer_ newBuffer = this->save_buffer(*pBuffer);
			std::swap(this->_pages.top(), newBuffer);
        }
        break;

        case Event::HIDE:
        {
            if(!this->buffer_empty(*pBuffer))
            {
                window->set_status_message("Changes not saved");
                break;
            }

            this->_pages.pop();
            window->hide();
        }
        break;

		case Event::SHOW:
		{
			if(this->_pages.empty()) this->_pages.push(this->load_buffer());
			Core::_buffer_* const pBuffer = this->current_buffer();
			pBuffer->window = id;

			this->update_window_buffer(window.get(), *pBuffer);
		}
		break;

		case Event::INSERT:
		{
			Core::_data_ val = this->create_empty_element();
			Core::_buffer_* pBuffer = this->current_buffer();
			pBuffer->_data.emplace_back(val);

			window->show_data(val.data);
		}
		break;

		default: break;
	}
}

void Core::event(std::string_view id, Event type, struct Data value) noexcept
{
    Core::_buffer_ last = *(this->current_buffer());
    auto begin = last._data.begin();
    auto end = last._data.end();
    auto row = std::find_if(begin, end, [id = value.index](auto val){ return val.data.index == id; });

    switch(type)
    {
		case Event::CHANGE:
		{
		    if(row->data.note != value.note || row->data.title != value.title)
            {
                row->status = Core::_status_::CHANGE;
                row->data = std::move(value);

                this->_pages.pop();
                auto& prev = this->_pages.top();
                last.window = prev.window;
                std::swap(last, prev);
                this->_pages.push(Core::_buffer_());

				this->update_window_buffer(prev.window, prev);
            }
		}
		break;

        default: break;
    }
}

void Core::event(std::string_view id, Event type, std::size_t index, WindowType window) noexcept
{
    Core::_buffer_* const pBuffer = this->current_buffer();
    Core::_data_& row = pBuffer->_data.at(index);

    switch(type)
    {
		case Event::OPEN:
		{
			auto weakPointer = this->_manager->get_window(window);
			auto windowPointer = weakPointer.lock();
			if(!windowPointer) return;

			if(window == WindowType::EDIT)
            {
                this->_pages.push(*pBuffer);
            }
			else
			{
				if(!this->buffer_empty(*pBuffer))
				{
					windowPointer->set_status_message("Changes not saved");
					return;
				}

                this->_pages.push(this->load_buffer(row.data.index));
			}

			windowPointer->modal(true);
			windowPointer->set_title(row.data.title);
			windowPointer->show();
		}
		break;

        default: break;
    }
}
