#include "../header/core.h"

#include <iostream>
#include <utility>
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
	std::vector<std::string> columns {"id", "data", "status", "parent"};
	std::vector<std::string> predicate {"deleted = 0", std::string("parent = ") + std::to_string(parent) };

	std::vector<std::unordered_map<std::string, std::string>> result = this->_database->query_select(this->_table, columns, predicate);

	std::vector<Core::_data_> buffer;
	buffer.reserve(result.size());

	for(const auto& row : result)
	{
		Data _row;
		_row.index = std::stoi(row.at("id"));
		_row.status = std::stoi(row.at("status"));
		_row.text = row.at("data");

		Core::_data_ _buffer;
		_buffer.data = _row;
		_buffer.created = Core::_created_::LOAD;
		_buffer.status = Core::_status_::NONE;
		
		buffer.emplace_back(_buffer);
	}

	return Core::_buffer_{parent, buffer};
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
					{"parent", "data", "status"},
					{std::to_string(buffer._id), row.data.text, std::to_string(row.data.status)});

			row.data.index = this->_database->last_insert_id();
		}

		if(row.created == Core::_created_::LOAD)
		{
			switch(row.status)
			{
				case Core::_status_::CHANGE:
				{
					this->_database->query_update(this->_table,
							{"data", "status"},
							{row.data.text, std::to_string(row.data.status)},
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

void Core::register_window(std::string_view id, IWindow* window) noexcept 
{
	this->_windowList.emplace_back(id, window);
}

void Core::event(std::string_view id, EventType type, std::size_t index) noexcept
{
	Core::_buffer_* const pBuffer = this->current_buffer();

	if(index >= pBuffer->_data.size()) return;

    _data_& row = pBuffer->_data.at(index);

    switch(type)
    {
		case EventType::ACTIVATE:
		{
			if(!row.data.status)
			{
				row.status = Core::_status_::CHANGE;
				row.data.status = true;
			}
		}
		break;

        case EventType::DEACTIVATE:
		{
			if(row.data.status)
			{
				row.status = Core::_status_::CHANGE;
				row.data.status = false;
			}
		}
		break;

		case EventType::DELETE:
		{
			row.status = Core::_status_::DELETE;
		}
		break;

        default: break;
    }
}

void Core::event(std::string_view id, EventType type) noexcept 
{
	auto window = get_element(this->_windowList, id);
	if(window == std::end(this->_windowList)) return;

	Core::_buffer_* const pBuffer = this->current_buffer();

	switch(type)
	{
        case EventType::SAVE:
        {
			Core::_buffer_ newBuffer = this->save_buffer(*pBuffer);
			std::swap(this->_pages.top(), newBuffer);
        }
        break;

        case EventType::HIDE:
        {
            if(!this->buffer_empty(*pBuffer))
            {
                if(window == std::end(this->_windowList)) break;
                window->second->set_status_message("Changes not saved");
                break;
            }

            this->_pages.pop();

            window->second->hide();
        }
        break;

		case EventType::SHOW:
		{
			if(this->_pages.empty()) this->_pages.push(this->load_buffer());

			const Core::_buffer_* const pBuffer = this->current_buffer();

			for(const auto& val : pBuffer->_data)
				window->second->show_data(val.data);
		}
		break;

		case EventType::INSERT:
		{
			Core::_data_ val = this->create_empty_element();
			Core::_buffer_* pBuffer = this->current_buffer();
			pBuffer->_data.emplace_back(val);

			window->second->show_data(val.data);
		}
		break;

		default: break;
	}
}

void Core::event(std::string_view id, EventType type, std::size_t index, std::string_view value) noexcept
{
    Core::_buffer_* const pBuffer = this->current_buffer();
    Core::_data_& row = pBuffer->_data.at(index);

    switch(type)
    {
		case EventType::CHANGE:
		{
			row.status = Core::_status_::CHANGE;
			row.data.text = value;
		}
		break;

		case EventType::OPEN:
		{
			auto window = get_element(this->_windowList, value);
			if(window == std::end(this->_windowList)) break;

            if(!this->buffer_empty(*pBuffer))
            {
				auto window = get_element(this->_windowList, id);
                if(window == std::end(this->_windowList)) break;
                window->second->set_status_message("Changes not saved");
                break;
            }

			window->second->modal(true);
			window->second->set_title(row.data.text);

			this->_pages.push(this->load_buffer(row.data.index));

			window->second->show();
		}
		break;

        default: break;
    }
}
