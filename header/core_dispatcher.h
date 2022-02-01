#ifndef DISPATCHER
#define DISPATCHER

#include "../interface/core_dispatcher.h"

class CoreDispatcher final : public ICoreDispatcher
{
	private:
		std::shared_ptr<ICoreEvent> _eventHandler;

	public:
        void set_handler(const std::shared_ptr<ICoreEvent>& handle) noexcept;

        const std::shared_ptr<ICoreEvent>& handler() noexcept override;
};

#endif //DISPATCHER

