#include "../header/core_dispatcher.h"


void CoreDispatcher::set_handler(const std::shared_ptr<ICoreEvent>& handle) noexcept
{
	this->_eventHandler = handle;
}

const std::shared_ptr<ICoreEvent>& CoreDispatcher::handler() noexcept
{
    return this->_eventHandler;
}
