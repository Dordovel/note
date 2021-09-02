#ifndef CORE
#define CORE

#include "../interface/core.h"
#include "../data.h"
#include "./database.h"

#include <glibmm-2.4/glibmm/refptr.h>
#include <gtkmm-3.0/gtkmm/application.h>
#include <gtkmm-3.0/gtkmm/window.h>


class Core final : public ICore
{
	private:
		enum _status_
		{
			CHANGE,
			DELETE,
			EMPTY,
			NONE
		};

		struct _data_
		{
			_status_ status;

			struct Data var;

			_data_(_status_ status, const Data& var): status(status),
														var(var){}

			_data_(_status_ status, std::size_t parent, const Data& var):status(status),
																		var(var){}
			_data_():status(_status_::EMPTY){}

			~_data_() = default;
		};

		struct _buffer_
		{
			std::string windowId;
			std::string parentId;
			std::string childrenId;
			std::string handler;

			bool subClass;
			bool parentClass;
			
			std::vector<_data_> bufferData;
		};

		_buffer_* create_default_buffer(std::string_view id) noexcept;
		_buffer_* create_sub_buffer(std::string_view parent, std::string_view id, std::string_view handler) noexcept;

		Core::_data_& create_default_element() noexcept;

		void save_buffer(_buffer_& buffer) noexcept;
		bool buffer_empty(const _buffer_& buffer) const noexcept;

		std::vector<std::pair<std::string, IWindow*>> _windowList;
		_buffer_* _pBuffer;
        std::vector<_buffer_> _buffers;
		std::shared_ptr<IDatabase> _database;

	public:
		Core(std::shared_ptr<IDatabase>  database);
		~Core() = default;

		void register_window(std::string_view id, IWindow* window) noexcept override;

		void event(std::string_view id, EventType type, std::size_t index) noexcept override;
		void event(std::string_view id, EventType type) noexcept override;
		void event(std::string_view id, EventType type, std::size_t index, std::string_view value) noexcept override;

};

#endif //CORE
