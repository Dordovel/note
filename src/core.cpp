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

Core::Core(std::shared_ptr<IDatabase> database):_database(std::move(database)), _pBuffer(nullptr)
{
}

Core::_buffer_* Core::create_default_buffer(std::string_view id) noexcept
{
	Core::_buffer_& newBuffer = this->_buffers.emplace_back();
	newBuffer.subClass = false;
	newBuffer.windowId = id;

	std::vector<std::string> columns {"id", "data", "status"};
	std::vector<std::string> predicate {"deleted = 0"};

	std::vector<std::vector<std::string>> result = this->_database->query_select(id, columns, predicate);

	if(!result.empty())
	{
		std::vector<Core::_data_> buffer;
		buffer.reserve(result.size());
		
		Core::_status_ status = Core::_status_::NONE;
		Data data;

		for(const auto& row : result)
		{
			data.index = std::stoi(row.at(0));
			data.text = row.at(1);
			data.status = std::stoi(row.at(2));

			buffer.emplace_back(status, data);
		}

		newBuffer.bufferData = std::move(buffer);
	}

	return &newBuffer;
}

Core::_buffer_* Core::create_sub_buffer(std::string_view parent, std::string_view id, std::string_view handler) noexcept
{
	Core::_buffer_& newBuffer = this->_buffers.emplace_back();
	newBuffer.parentId = parent;
	newBuffer.subClass = true;
	newBuffer.windowId = id;
	newBuffer.handler = handler;

	std::vector<std::string> columns {"id", "data", "status"};
	std::vector<std::string> predicate {"deleted = 0", std::string("parent_id = ") + std::string(handler)};

	std::vector<std::vector<std::string>> result = this->_database->query_select(id, columns, predicate);

	if(!result.empty())
	{
		std::vector<Core::_data_> buffer;
		buffer.reserve(result.size());
		
		Core::_status_ status = Core::_status_::NONE;
		Data data;

		for(const auto& row : result)
		{
			data.index = std::stoi(row.at(0));
			data.text = row.at(1);
			data.status = std::stoi(row.at(2));

			buffer.emplace_back(status, data);
		}

		newBuffer.bufferData = std::move(buffer);
	}

	return &this->_buffers.back();
}

Core::_data_& Core::create_default_element() noexcept
{
	int index = 0;

	if(this->_pBuffer)
	{
		if(!this->_pBuffer->bufferData.empty())
		{
			index = ( this->_pBuffer->bufferData.back().var.index + 1 );
		}
	}

	Core::_data_ data;
	data.var.index = index;
	this->_pBuffer->bufferData.push_back(data);

	return this->_pBuffer->bufferData.back();
}

void Core::save_buffer(Core::_buffer_& buffer) noexcept
{
	for(auto& row : buffer.bufferData)
	{
		switch(row.status)
		{
			case _status_::DELETE:
			{
				if(buffer.subClass)
				{
					this->_database->query_update(buffer.windowId,
							{"deleted"},
							{"1"},
							{"parent_id = " + buffer.handler});
				}
				else
				{
					this->_database->query_update(buffer.windowId,
							{"deleted"},
							{"1"},
							{"id = " + std::to_string(row.var.index)});

					if(buffer.parentClass)
					{
						this->_database->query_update(buffer.childrenId,
								{"deleted"},
								{"1"},
								{"parent_id = " + buffer.handler});
					}
					
				}

			}
			break;

			case _status_::EMPTY:
				{
					if(buffer.subClass)
					{
						this->_database->query_insert(buffer.windowId,
								{"parent_id", "data", "status"},
								{buffer.handler, row.var.text, std::to_string(row.var.status)});
					}
					else
					{
						this->_database->query_insert(buffer.windowId,
								{"data", "status"},
								{row.var.text, std::to_string(row.var.status)});
					}
				}
			break;

			case Core::_status_::CHANGE:
			{
				this->_database->query_update(buffer.windowId,
						{"data", "status"},
						{row.var.text, std::to_string(row.var.status)},
						{"id = " + std::to_string(row.var.index)});
			}
			break;

			default: break;
		}

		row.status = Core::_status_::NONE;

	}

}

bool Core::buffer_empty(const Core::_buffer_& buffer) const noexcept
{
	auto begin = buffer.bufferData.begin();
	auto end = buffer.bufferData.end();
	int changes = std::count_if(begin, end, [](auto&& val){return val.status != Core::_status_::NONE;});
	
	return 0 == changes;
}

void Core::register_window(std::string_view id, IWindow* window) noexcept 
{
    this->_windowList.emplace_back(id, window);
}

void Core::event(std::string_view id, EventType type, std::size_t index) noexcept
{
	if(index >= this->_pBuffer->bufferData.size()) return;

    _data_& row = this->_pBuffer->bufferData.at(index);

    switch(type)
    {
        case EventType::ACTIVATE:
			{
				if(!row.var.status)
				{
					row.status = Core::_status_::CHANGE;
					row.var.status = true;
				}
			}
		break;

        case EventType::DEACTIVATE:
			{
				if(row.var.status)
				{
					row.status = Core::_status_::CHANGE;
					row.var.status = false;
				}
			}
		break;

		case EventType::DELETE:
			{
				if(row.status == Core::_status_::DELETE)
					this->event(id, type, index + 1);

				if (row.status == Core::_status_::EMPTY)
					row.status = Core::_status_::NONE;
				else
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

	switch(type)
	{
		case EventType::SAVE:
			{
				this->save_buffer(*this->_pBuffer);
			}
		break;

        case EventType::HIDE:
			{
				if(!this->buffer_empty(*this->_pBuffer))
				{
					window->second->set_status_message("Changes not saved");
					break;
				}

				if(this->_pBuffer)
					this->_buffers.erase(std::prev(std::end(this->_buffers)));

				if(!this->_buffers.empty())
					this->_pBuffer = &this->_buffers.back();

				this->_pBuffer->childrenId.clear();
				this->_pBuffer->parentClass = false;

				window->second->hide();

			}
		break;

		case EventType::SHOW:
			{
				if(!this->_pBuffer)
					if(this->_pBuffer = this->create_default_buffer(id); !this->_pBuffer) break;

				for(const auto& val : this->_pBuffer->bufferData)
					window->second->show_data(val.var);
			}
		break;

		case EventType::INSERT:
			{
				Core::_data_ val = this->create_default_element();
				window->second->show_data(val.var);
			}
		break;

		default: break;
	}

}

void Core::event(std::string_view id, EventType type, std::size_t index, std::string_view value) noexcept
{
	if(value.empty()) return;

    switch(type)
    {
        case EventType::CHANGE:
		{
			Core::_data_& row = this->_pBuffer->bufferData.at(index);

			if(row.status != Core::_status_::EMPTY)
				row.status = Core::_status_::CHANGE;

			row.var.text = value;
		}
		break;

		case EventType::OPEN:
		{
			if(!this->buffer_empty(*this->_pBuffer))
			{
				auto parent = get_element(this->_windowList, id);
				if(parent == std::end(this->_windowList)) break;
				parent->second->set_status_message("Changes not saved");
				break;
			}

			auto window = get_element(this->_windowList, value);
			if(window == std::end(this->_windowList)) break;

			window->second->modal(true);
			window->second->set_title(this->_pBuffer->bufferData.at(index).var.text);

			this->_pBuffer->childrenId = value;
			this->_pBuffer->parentClass = true;
			this->_pBuffer = this->create_sub_buffer(id, value,
											std::to_string(this->_pBuffer->bufferData.at(index).var.index));

			window->second->show();
		}
		break;

        default: break;
    }
}
