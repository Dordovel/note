#ifndef EVENT_INTERFACE
#define EVENT_INTERFACE

#include <string_view>

enum class CoreEventTypes
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
struct Block;

class ICoreEvent
{
	public:

		virtual void event(std::string_view id, CoreEventTypes type, std::size_t index) noexcept = 0;
		virtual void event(std::string_view id, CoreEventTypes type) noexcept = 0;
		virtual void event(std::string_view id, CoreEventTypes type, struct Block value) noexcept = 0;
		virtual void event(std::string_view id, CoreEventTypes type, std::size_t index, WindowType window) noexcept = 0;
};

#endif //EVENT_INTERFACE
