#ifndef WINDOW_INTERFACE
#define WINDOW_INTERFACE

#include "./dispatcher.h"

#include <string>
#include <string_view>
#include <vector>

struct data;

class IWindow
{
	public:

		virtual void show() = 0;
		virtual void hide() = 0;

		virtual void set_title(std::string_view title) noexcept = 0;
		virtual std::string get_title() const noexcept = 0;

		virtual void set_style(std::string_view path) noexcept = 0;

		virtual void set_size(int width, int height) noexcept = 0;

		virtual void set_name(std::string_view name) noexcept = 0;
		virtual std::string get_name() const noexcept = 0;

		virtual void add_data(std::vector<struct data> data) noexcept = 0;
		virtual void add_data(struct data) noexcept = 0;

		virtual std::vector<struct data> get_data() const noexcept = 0;

		virtual ~IWindow() = default;

};

#endif //WINDOW_INTERFACE
