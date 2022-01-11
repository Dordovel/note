#include "../header/dispatcher.h"


void Dispatcher::set_handler(const std::shared_ptr<IEvent>& handle) noexcept
{
	this->_eventHandler = handle;
}

const std::shared_ptr<IEvent>& Dispatcher::handler() noexcept
{
    return this->_eventHandler;
}
