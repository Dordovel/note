#ifndef WINDOW
#define WINDOW

#include "./dispatcher.h"

#include <string>
#include <string_view>
#include <vector>

struct data;

class IWindow
{
	public:
		virtual void set_title(std::string_view title) noexcept = 0;
		virtual std::string get_title() const noexcept = 0;

		virtual void set_name(std::string_view name) noexcept = 0;
		virtual std::string get_name() const noexcept = 0;

		virtual void set_dispatcher(const std::shared_ptr<IDispatcher>& dispather) noexcept = 0;

		virtual void add_data(std::vector<struct data> data) noexcept = 0;
		virtual void add_data(struct data) noexcept = 0;

		virtual std::vector<struct data> get_data() const noexcept = 0;

		virtual ~IWindow() = default;

};

#endif //WINDOW
