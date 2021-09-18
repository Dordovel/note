#ifndef CORE_INTERFACE
#define CORE_INTERFACE

#include "./window.h"
#include "./event.h"

#include "window_manager.hpp"

class ICore : public IEvent
{
	public:
		virtual void register_manager(std::unique_ptr<IWindowRegisterGet> manager) noexcept = 0;

		virtual ~ICore() = default;

};

#endif //CORE_INTERFACE
