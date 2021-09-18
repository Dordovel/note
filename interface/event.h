#ifndef EVENT_INTERFACE
#define EVENT_INTERFACE

#include <string_view>

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
	UPDATE
};

enum class WindowType;
struct Data;

class IEvent
{
	public:

		virtual void event(std::string_view id, Event type, std::size_t index) noexcept = 0;
		virtual void event(std::string_view id, Event type) noexcept = 0;
		virtual void event(std::string_view id, Event type, struct Data value) noexcept = 0;
		virtual void event(std::string_view id, Event type, std::size_t index, WindowType window) noexcept = 0;
};

#endif //EVENT_INTERFACE
