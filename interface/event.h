#ifndef EVENT
#define EVENT

#include <string_view>
#include <vector>

enum EventType
{
	CLOSE,
	CHANGE,
	DELETE,
	ADD
};

class IEvent
{
	public:

		virtual int event(std::string_view id, EventType type, int value) noexcept = 0;
		virtual int event (std::string_view id, EventType type) noexcept = 0;
		virtual int event (std::string_view id, EventType type, const std::vector<struct data> value) noexcept = 0;
};

#endif //EVENt
