#ifndef CORE
#define CORE

#include "../interface/core.h"
#include "../data.h"
#include "./database.h"

#include <glibmm-2.4/glibmm/refptr.h>
#include <gtkmm-3.0/gtkmm/application.h>
#include <gtkmm-3.0/gtkmm/window.h>
#include <stack>


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

		struct _data_
		{
			_status_ status;
			_created_ created;

			struct Data data;
		};

		struct _buffer_
		{
			int _id;
			std::vector<_data_> _data;
		};

		_buffer_ load_buffer(int parent = -1) noexcept;

		Core::_data_ create_empty_element() noexcept;

		_buffer_ save_buffer(_buffer_ buffer) const noexcept;
		bool buffer_empty(const _buffer_& buffer) const noexcept;
		_buffer_* current_buffer() noexcept;

		std::vector<std::pair<std::string, IWindow*>> _windowList;
		std::stack<_buffer_> _pages;
		std::shared_ptr<IDatabase> _database;

		std::string _table = "main";

	public:
		Core(std::shared_ptr<IDatabase>  database);
		~Core() = default;

		void register_window(std::string_view id, IWindow* window) noexcept override;

		void event(std::string_view id, Event type, std::size_t index) noexcept override;
		void event(std::string_view id, Event type) noexcept override;
		void event(std::string_view id, Event type, std::size_t index, std::string_view value) noexcept override;

};

#endif //CORE
