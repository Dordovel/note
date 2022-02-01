
#ifndef COMPONENT_EVENT_INTERFACE
#define COMPONENT_EVENT_INTERFACE

#include <string_view>

enum class ComponentEventTypes
{
	ACTIVATE,
	DEACTIVATE,
	SELECT
};

class IComponentEvent
{
	public:
		virtual void event(ComponentEventTypes type, std::size_t index) noexcept = 0;
};

#endif //COMPONENT_EVENT_INTERFACE

