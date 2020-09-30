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

