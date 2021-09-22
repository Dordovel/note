#ifndef WINDOW_INTERFACE
#define WINDOW_INTERFACE

#include <string>
#include <vector>

struct Data;

class IWindow
{
	public:

		virtual void open_window() = 0;
		virtual void close_window() = 0;

		virtual void modal(bool flag) noexcept = 0;

		virtual void set_window_title(std::string_view title) noexcept = 0;
		virtual std::string get_window_title() const noexcept = 0;

		virtual void set_status_message(std::string_view status) noexcept = 0;

		virtual void set_window_css_file_path(std::string_view path) noexcept = 0;

		virtual void set_window_size(int width, int height) noexcept = 0;

		virtual void set_window_id(std::string_view name) noexcept = 0;
		virtual std::string get_window_id() const noexcept = 0;

		virtual void show_data_in_window(const struct Data& value) noexcept = 0;

		virtual void clear_window() noexcept = 0;
		
		virtual ~IWindow() = default;

};

#endif //WINDOW_INTERFACE
