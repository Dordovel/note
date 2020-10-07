#ifndef CORE
#define CORE

#include "../interface/core.h"
#include "../data.h"

#include <glibmm-2.4/glibmm/refptr.h>
#include <gtkmm-3.0/gtkmm/application.h>
#include <gtkmm-3.0/gtkmm/window.h>


class Core final : public ICore
{
	private:
		enum _status_
		{
			NONE,
			CHANGE,
			DELETE,
			INSERT
		};

		struct _data_
		{
			_status_ status;
			struct data var;

			_data_(_status_ status, const data& var): status(status), var(var){}
		};

		std::vector<std::pair<std::string, IWindow*>> _windowList;
		std::vector<_data_> _data;

	public:
		Core();
		~Core() = default;

		void register_window(std::string_view id, IWindow* window) noexcept override;

		int event(std::string_view id, EventType type, std::string_view value) noexcept override;
		int event(std::string_view id, EventType type) noexcept override;
		int event(std::string_view id, EventType type, std::string_view value, std::string_view value1) noexcept override;

};

#endif //CORE
