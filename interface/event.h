#ifndef EVENT_INTERFACE
#define EVENT_INTERFACE

#include <string_view>
#include <vector>

enum Event
{
	SHOW,
	HIDE,
	OPEN,
	CHANGE,
	DELETE,
	INSERT,
	ACTIVATE,
	DEACTIVATE,
	SAVE,
	UPDATE,
};

class IEvent
{
	public:

		virtual void event(std::string_view id, Event type, std::size_t index) noexcept = 0;
		virtual void event(std::string_view id, Event type) noexcept = 0;
		virtual void event(std::string_view id, Event type, std::size_t index, std::string_view value) noexcept = 0;
};

#endif //EVENT_INTERFACE
