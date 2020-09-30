#include "../header/core.h"
#include "../data.h"

#include <iostream>


void Core::register_window(std::string_view id, IWindow* window) noexcept 
{
    this->_windowList.emplace_back(id, window);
}

int Core::event(std::string_view id, EventType type, std::string_view value) noexcept
{
    switch(type)
    {
        case EventType::ACTIVATE:
                std::cout<<"Core Event: Toggle Activate\n";
            break;
        case EventType::DEACTIVATE:
                std::cout<<"Core Event: Toggle Deactivate\n";
            break;
        default:
            break;
    }
    return 0;
}

int Core::event(std::string_view id, EventType type) noexcept 
{
	switch(type)
	{
		case EventType::SAVE:
			{
				auto window = std::find_if(std::begin(this->_windowList), std::end(this->_windowList), 
						[id=id](auto val){return val.first == id;});

				if(window != std::end(this->_windowList))
				{
					std::vector<struct data> data = (*window).second->get_data();
					for(const auto& val : data)
					{
						std::cout<<val.text<<std::endl;
					}
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
            std::cout<<"Core Event: Changed {Id: "<<value<<" Change value: "<<value1<<"}\n";
        default:
            break;
    }
    return 0;
}

