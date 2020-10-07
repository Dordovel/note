#include "../header/core.h"

#include <iostream>

Core::Core()
{
	this->_data.emplace_back(Core::_status_::INSERT, data{1, "1", true});
	this->_data.emplace_back(Core::_status_::INSERT, data{2, "2", true});
	this->_data.emplace_back(Core::_status_::INSERT, data{3, "3", false});
	this->_data.emplace_back(Core::_status_::INSERT, data{4, "4", true});
	this->_data.emplace_back(Core::_status_::INSERT, data{5, "5", false});
}

void Core::register_window(std::string_view id, IWindow* window) noexcept 
{
    this->_windowList.emplace_back(id, window);
}

int Core::event(std::string_view id, EventType type, std::string_view value) noexcept
{
	int index = std::stoi(value.data());
	auto row = std::find_if(std::begin(this->_data), std::end(this->_data), [index](auto value)
				{return value.var.index == index;});
	if(row == std::end(this->_data))
		return -1;

    switch(type)
    {
        case EventType::ACTIVATE:
			{
				if(!row->var.status)
				{
					row->status = Core::_status_::CHANGE;
					row->var.status = true;
				}
			}
            break;
        case EventType::DEACTIVATE:
			{
				if(row->var.status)
				{
					row->status = Core::_status_::CHANGE;
					row->var.status = false;
				}
			}
            break;
		case EventType::DELETE:
				row->status = Core::_status_::DELETE;
			break;

        default:
            break;
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
				for(const auto& val : this->_data)
				{
					std::cout<<"Value: "<<val.var.index<<" "<<val.var.text<<std::endl;
				}
			}
			break;
		case EventType::UPDATE:
			{
				if(window != std::end(this->_windowList))
				{
					for(const auto& val : this->_data)
					{
						window->second->show_data(val.var);
					}
				}
			}
			break;
		case EventType::INSERT:
			{
				if(window != std::end(this->_windowList))
				{
					const std::size_t size = this->_data.size();
					int index = (this->_data.at(size - 1).var.index + 1);
					Core::_data_ val = this->_data.emplace_back(Core::_status_::INSERT, data{index, "", false});
					
					window->second->show_data(val.var);
				}
			}
			break;

		default:
			break;
	}

    return 0;
}

int Core::event(std::string_view id, EventType type, std::string_view value, std::string_view value1) noexcept
{
    switch(type)
    {
        case EventType::CHANGE:
			{
				int index = std::stoi(value.data());

				auto row = std::find_if(std::begin(this->_data), std::end(this->_data), [index](auto value)
						{return value.var.index == index;});

				if(row != std::end(this->_data))
				{
					if(row->var.text != value1)
					{
						row->status = Core::_status_::CHANGE;
						row->var.text = value1;
					}
				}
			}
        default:
            break;
    }
    return 0;
}

