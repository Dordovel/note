#ifndef CORE
#define CORE

#include "../interface/core.h"

#include <glibmm-2.4/glibmm/refptr.h>
#include <gtkmm-3.0/gtkmm/application.h>
#include <gtkmm-3.0/gtkmm/window.h>


class Core : public ICore
{
	private:
		std::vector<std::pair<std::string, IWindow*>> _windowList;

	public:
		void register_window(std::string_view id, IWindow* window) noexcept override;

		int event(std::string_view id, EventType type, std::string_view value) noexcept override;
		int event(std::string_view id, EventType type) noexcept override;
		int event(std::string_view id, EventType type, std::string_view value, std::string_view value1) noexcept override;

};

#endif //CORE
