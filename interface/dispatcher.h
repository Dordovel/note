#ifndef DISPATCHER
#define DISPATCHER

#include "./event.h"

#include <memory>

class IDispatcher : public IEvent
{
	public:
		virtual void add_event_handle(const std::shared_ptr<IEvent>& handle) noexcept = 0;

		virtual ~IDispatcher() = default;
};

#endif //DISPATCHER
