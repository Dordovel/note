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

			case _status_::INSERT:
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
			break;

			case Core::_status_::CHANGE:
				{
					this->_database->query_update(buffer.windowId,
							{"data", "status"},
							{row.var.text, std::to_string(row.var.status)},
							{"id = " + std::to_string(row.var.index)});
				}
			break;

			default:
				std::cout<<"DEFAULT: "<<row.status<<std::endl;
			break;
		}

	}

	row.status = Core::_status_::NONE;
	}
}

void Core::register_window(std::string_view id, IWindow* window) noexcept 
{
    this->_windowList.emplace_back(id, window);
}

int Core::event(std::string_view id, EventType type, std::size_t index) noexcept
{
	if(index >= this->_pBuffer->bufferData.size()) return -1;

    _data_& row = this->_pBuffer->bufferData.at(index);

    switch(type)
    {
        case EventType::ACTIVATE:
			{
				if(!row.var.status)
				{
					if(row.status != Core::_status_::INSERT)
						row.status = Core::_status_::CHANGE;

					row.var.status = true;
				}
			}
		break;

        case EventType::DEACTIVATE:
			{
				if(row.var.status)
				{
					if(row.status != Core::_status_::INSERT)
						row.status = Core::_status_::CHANGE;

					row.var.status = false;
				}
			}
		break;

		case EventType::DELETE:
			{
				if(row.status == Core::_status_::DELETE)
					this->event(id, type, index + 1);

				row.status = Core::_status_::DELETE;
			}
		break;

        default: break;
    }
    return 0;
}

int Core::event(std::string_view id, EventType type) noexcept 
{
	auto window = get_element(this->_windowList, id);

	switch(type)
	{
		case EventType::SAVE:
			{
				this->save_buffer(*this->_pBuffer);
			}
		break;

        case EventType::HIDE:
			{
				if(this->_pBuffer)
					this->_buffers.erase(std::prev(std::end(this->_buffers)));

				if(!this->_buffers.empty())
					this->_pBuffer = &this->_buffers.back();

				this->_pBuffer->childrenId.clear();
				this->_pBuffer->parentClass = false;
			}
		break;

		case EventType::SHOW:
			{
				if(!this->_pBuffer)
					if(this->_pBuffer = this->create_default_buffer(id); !this->_pBuffer) break;

				if(window != std::end(this->_windowList))
					for(const auto& val : this->_pBuffer->bufferData)
						window->second->show_data(val.var);
			}
		break;

		case EventType::INSERT:
			{
				if(window != std::end(this->_windowList))
				{
					Core::_data_ val = this->create_default_element();
					
					window->second->show_data(val.var);
				}
			}
		break;

		case EventType::CHANGE_LIST:
			{
				int changes = 0;
				auto begin = std::begin(this->_pBuffer->bufferData);
				auto end = std::end(this->_pBuffer->bufferData);

				std::for_each(begin, end, [&changes](auto a){if (a.status != Core::_status_::NONE) ++changes;});

				return changes;
			}
		break;

		default: break;
	}

    return 0;
}

int Core::event(std::string_view id, EventType type, std::size_t index, std::string_view value) noexcept
{
    switch(type)
    {
        case EventType::CHANGE:
			{
			    Core::_data_& row = this->_pBuffer->bufferData.at(index);

                if(row.var.text != value)
				{
                    row.var.text = value;

					if(row.status != Core::_status_::INSERT)
						row.status = Core::_status_::CHANGE;
				}
			}
		break;

		case EventType::OPEN:
				{
					auto window = get_element(this->_windowList, value);
					if(window != std::end(this->_windowList))
                    {
						window->second->modal(true);
						window->second->set_title(this->_pBuffer->bufferData.at(index).var.text);

						this->_pBuffer->childrenId = value;
						this->_pBuffer->parentClass = true;
						this->_pBuffer = this->create_sub_buffer(id, value,
														std::to_string(this->_pBuffer->bufferData.at(index).var.index));

                        window->second->show();
					}
				}
		break;

        default: break;
    }
    return 0;
}
