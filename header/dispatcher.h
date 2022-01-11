#ifndef DISPATCHER
#define DISPATCHER

#include "../interface/dispatcher.h"

class Dispatcher final : public IDispatcher
{
	private:
		std::shared_ptr<IEvent> _eventHandler;

	public:
        void set_handler(const std::shared_ptr<IEvent>& handle) noexcept;

        const std::shared_ptr<IEvent>& handler() noexcept override;
};

#endif //DISPATCHER

