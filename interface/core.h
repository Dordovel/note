#ifndef CORE_INTERFACE
#define CORE_INTERFACE

#include "./window.h"
#include "./event.h"

#include <memory>

class ICore : public IEvent
{
	public:
		virtual void register_window(std::string_view id, IWindow* window) noexcept = 0;

		virtual ~ICore() = default;

};

#endif //CORE_INTERFACE
