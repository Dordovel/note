#ifndef EVENT_INTERFACE
#define EVENT_INTERFACE

#include <string_view>
#include <vector>

enum EventType
{
	CLOSE,
	OPEN,
	CHANGE,
	DELETE,
	INSERT,
	ACTIVATE,
	DEACTIVATE
};

class IEvent
{
	public:

		virtual int event(std::string_view id, EventType type, std::string_view value) noexcept = 0;
		virtual int event(std::string_view id, EventType type) noexcept = 0;
		virtual int event(std::string_view id, EventType type, std::string_view value, std::string_view value1) noexcept = 0;
};

#endif //EVENT_INTERFACE
