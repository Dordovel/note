#ifndef DISPATCHER_INTERFACE
#define DISPATCHER_INTERFACE

#include "./core_event.h"

#include <memory>

class ICoreDispatcher
{
	public:
		virtual const std::shared_ptr<ICoreEvent>& handler() noexcept = 0;

		virtual ~ICoreDispatcher() = default;
};

#endif //DISPATCHER_INTERFACE
