#include "../header/core.h"

#include <iostream>
#include <utility>
#include <algorithm>

Core::Core(std::shared_ptr<IDatabase> database):_database(std::move(database))
{
}

std::vector<Core::_data_> Core::load_data(std::string_view table, std::vector<std::string> predicate) noexcept
{
	std::vector<std::string> columns {"id", "data", "status"};
	std::vector<std::vector<std::string>> data = this->_database->query_select(table, columns, predicate);

    std::vector<Core::_data_> buffer;
    buffer.reserve(data.size());

	for(const auto& row : data)
	{
		buffer.emplace_back(Core::_status_::NONE, Data(std::stoi(row.at(0)),
                                                                  row.at(1),
                                                                  std::stoi(row.at(2))));
	}

	return buffer;
}

std::vector<Core::_data_>* Core::init_buffer(std::string_view id) noexcept
{
	std::vector<Core::_data_> data = this->load_data(id, {"deleted = 0"});
	
	if(!data.empty())
	{
		this->_data.push_back(data);
		return &this->_data.at(this->_data.size() - 1);
	}

	return nullptr;
}

Core::_data_ Core::create_default_element() noexcept
{
	int index = 0;

	if(this->_pData)
	{
		if(!this->_pData->empty())
		{
			const std::size_t size = this->_pData->size();
			index = ( this->_pData->at(size - 1).var.index + 1 );
		}
	}
	else
	{
		this->_pData = &this->_data.emplace_back(decltype(this->_data)::value_type{});
	}

	return this->_pData->emplace_back();
}

void Core::save_buffer(std::string_view table, const std::vector<Core::_data_>& buffer) noexcept
{
	for(const auto& row : buffer)
	{
		switch(row.status)
		{
			case Core::_status_::DELETE:
				{
					this->_database->query_delete(table, {"id = " + std::to_string(row.var.index)});
				}
			break;

			default:
				std::cout<<"DEFAULT: "<<row.status<<std::endl;
			break;
		}
	}
}

void Core::register_window(std::string_view id, IWindow* window) noexcept 
{
    this->_windowList.emplace_back(id, window);
}

int Core::event(std::string_view id, EventType type, std::size_t index) noexcept
{
    _data_ row = this->_pData->at(index);

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
				row.status = Core::_status_::DELETE;
		break;

		case EventType::OPEN:
				{
					auto window = std::find_if(std::begin(this->_windowList), std::end(this->_windowList), [](auto window){return window.first == "edit";});
					if(window != std::end(this->_windowList))
                    {
					    this->_pData = &this->_dataEdit;
						window->second->modal(true);
						window->second->set_title(std::to_string(index));
						
                        window->second->show();
					}
				}

		break;

        default: break;
    }
    return 0;
}

int Core::event(std::string_view id, EventType type) noexcept 
{
	auto window = std::find_if(std::begin(this->_windowList), std::end(this->_windowList), [id] (auto value)
			{return value.first == id;});

	switch(type)
	{
		case EventType::SAVE:
			{
				this->save_buffer(id, *this->_pData);
				for(const auto& val : (*this->_pData))
					std::cout<<"Value: "<<val.var.index<<" "<<val.var.text<<std::endl;
			}
		break;

        case EventType::HIDE:
			{
				this->_data.erase(std::prev(std::end(this->_data)));

				if(!this->_data.empty())
					this->_pData = &this->_data.at(this->_data.size() - 1);
			}

		break;

		case EventType::SHOW:
			{
				if(this->_pData = this->init_buffer(id); !this->_pData) break;

				if(window != std::end(this->_windowList))
					for(const auto& val : (*this->_pData))
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
			    Core::_data_ row = this->_pData->at(index);

                if(row.var.text != value)
                {
                    row.status = Core::_status_::CHANGE;
                    row.var.text = value;
                }
			}
		break;

        default: break;
    }
    return 0;
}

