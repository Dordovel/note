#ifndef WINDOW_INTERFACE
#define WINDOW_INTERFACE

#include <string>
#include <vector>

struct Data;

class IWindow
{
	public:

		virtual void show() = 0;
		virtual void hide() = 0;

		virtual void modal(bool flag) noexcept = 0;

		virtual void set_title(std::string_view title) noexcept = 0;
		virtual std::string get_title() const noexcept = 0;

		virtual void set_status_message(std::string_view status) noexcept = 0;

		virtual void set_style(std::string_view path) noexcept = 0;

		virtual void set_size(int width, int height) noexcept = 0;

		virtual void set_name(std::string_view name) noexcept = 0;
		virtual std::string get_name() const noexcept = 0;

		virtual void show_data(const struct Data& value) noexcept = 0;

		virtual void clear() noexcept = 0;
		
		virtual ~IWindow() = default;

};

#endif //WINDOW_INTERFACE
