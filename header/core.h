#ifndef CORE
#define CORE

#include "../interface/core.h"
#include "../data.h"
#include "./database.h"
#include "glibmm/spawn.h"
#include "page.h"
#include "page_factory.h"
#include "page_saver.h"

#include <glibmm-2.4/glibmm/refptr.h>
#include <gtkmm-3.0/gtkmm/application.h>
#include <gtkmm-3.0/gtkmm/window.h>
#include <stack>
#include <unordered_map>

class Core final : public ICore
{
	private:

		struct _window_
		{
			int level = 0;
			std::unordered_map<std::size_t, std::size_t> _blockReferer;
			std::string _windowId;
			std::weak_ptr<IWindow> _window;
			std::shared_ptr<IPage> _page;
		};

		void update_window(Core::_window_& window) noexcept;

		std::stack<Core::_window_> _windows;
		std::unique_ptr<IWindowRegisterGet> _manager;

		std::unique_ptr<PageFactory> _pageFactory;
		std::unique_ptr<PageSaver> _pageSaver;

	public:

		void register_manager(std::unique_ptr<IWindowRegisterGet> manager) noexcept override;
		void register_page_factory(std::unique_ptr<PageFactory> factory);
		void register_page_saver(std::unique_ptr<PageSaver> saver);

		void event(std::string_view id, CoreEventTypes type, std::size_t index) noexcept override;
		void event(std::string_view id, CoreEventTypes type) noexcept override;
		void event(std::string_view id, CoreEventTypes type, struct Block value) noexcept override;
		void event(std::string_view id, CoreEventTypes type, std::size_t index, WindowType window) noexcept override;
};

#endif //CORE
