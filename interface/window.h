#ifndef WINDOW_INTERFACE
#define WINDOW_INTERFACE

#include <string>
#include <vector>

struct Data;

class IWindow
{
	public:

		virtual void open() = 0;
		virtual void close() = 0;

		virtual void modal(bool flag) noexcept = 0;

		virtual void set_title(std::string_view title) noexcept = 0;
		virtual std::string title() const noexcept = 0;

		virtual void set_info_message(std::string_view status) noexcept = 0;

		virtual void set_path_to_css_file(std::string_view path) noexcept = 0;

		virtual void set_size(int width, int height) noexcept = 0;

		virtual void set_id(std::string_view name) noexcept = 0;
		virtual std::string id() const noexcept = 0;

		virtual void print(const struct Data& value) noexcept = 0;

		virtual void clear() noexcept = 0;
		
		virtual ~IWindow() = default;

};

#endif //WINDOW_INTERFACE
