#ifndef WINDOW_INTERFACE
#define WINDOW_INTERFACE


#include <string>
#include <string_view>
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

		virtual void set_style(std::string_view path) noexcept = 0;

		virtual void set_size(int width, int height) noexcept = 0;

		virtual void set_name(std::string_view name) noexcept = 0;
		virtual std::string get_name() const noexcept = 0;

		virtual void show_data(const struct Data& value) noexcept = 0;

		virtual void set_sub_window(std::string_view id) noexcept = 0;
		virtual std::string get_sub_window() noexcept = 0;
		
		virtual ~IWindow() = default;

};

#endif //WINDOW_INTERFACE
