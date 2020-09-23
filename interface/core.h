#ifndef CORE
#define CORE

#include "./window.h"
#include "./event.h"

#include <memory>

class ICore : public IEvent
{
	public:
		virtual void register_window(std::string_view id, std::unique_ptr<IWindow> window) noexcept = 0;

		virtual bool is_active() const noexcept = 0;

		virtual void close() noexcept = 0;

		virtual ~ICore() = default;

};

#endif //CORE
