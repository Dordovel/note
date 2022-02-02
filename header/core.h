#ifndef CORE
#define CORE

#include "../interface/core.h"
#include "../data.h"
#include "./database.h"
#include "glibmm/spawn.h"

#include <glibmm-2.4/glibmm/refptr.h>
#include <gtkmm-3.0/gtkmm/application.h>
#include <gtkmm-3.0/gtkmm/window.h>
#include <stack>
#include <optional>

class Core final : public ICore
{
	private:
		enum _status_
		{
			CHANGE,
			DELETE,
			NONE
		};
		
		enum _created_
		{
			LOAD,
			NEW
		};

		struct _line_
		{
			_status_ status;
			_created_ created;

			struct Block block;
		};

		struct _page_
		{
			int _id;
			std::string window;
			std::vector<_line_> _lines;
		};

		Core::_page_ load_page_from_db(int parent = -1) noexcept;
		Core::_line_ create_empty_element() noexcept;
		Core::_page_ save_page(_page_ buffer) const noexcept;
		Core::_page_* current_page() noexcept;
		bool is_change(const _page_& buffer) const noexcept;
        void update_window(std::string_view window, const Core::_page_& buffer) noexcept;
		void update_window(IWindow* window, const Core::_page_& buffer) noexcept;

		std::stack<_page_> _pages;
		std::unique_ptr<IWindowRegisterGet> _manager;
		std::shared_ptr<IDatabase> _database;

		std::string _table = "main";

	public:
		explicit Core(std::shared_ptr<IDatabase>  database);
		~Core() = default;

		void register_manager(std::unique_ptr<IWindowRegisterGet> manager) noexcept override;

		void event(std::string_view id, CoreEventTypes type, std::size_t index) noexcept override;
		void event(std::string_view id, CoreEventTypes type) noexcept override;
		void event(std::string_view id, CoreEventTypes type, struct Block value) noexcept override;
		void event(std::string_view id, CoreEventTypes type, std::size_t index, WindowType window) noexcept override;
};

#endif //CORE
