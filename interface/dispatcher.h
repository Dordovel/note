#ifndef DISPATCHER_INTERFACE
#define DISPATCHER_INTERFACE

#include "./event.h"

#include <memory>

class IDispatcher
{
	public:
		virtual const std::shared_ptr<IEvent>& handler() noexcept = 0;

		virtual ~IDispatcher() = default;
};

#endif //DISPATCHER_INTERFACE
