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
			NONE,
			CHANGE,
			DELETE,
			INSERT
		};

		struct _data_
		{
			_status_ status;
			struct Data var;

			_data_(_status_ status, const Data& var): status(status), var(var){}
			_data_():status(_status_::INSERT), var(){}

			~_data_() = default;
		};

		std::vector<Core::_data_> load_data(std::string_view table, std::vector<std::string> predicate) noexcept;

		std::vector<_data_>* init_buffer(std::string_view id) noexcept;
		Core::_data_ create_default_element() noexcept;

		void save_buffer(std::string_view table, const std::vector<_data_>& buffer) noexcept;

		std::vector<std::pair<std::string, IWindow*>> _windowList;
		std::vector<_data_>* _pBuffer;
        std::vector<std::vector<_data_>> _buffers;
		std::shared_ptr<IDatabase> _database;

	public:
		Core(std::shared_ptr<IDatabase>  database);
		~Core() = default;

		void register_window(std::string_view id, IWindow* window) noexcept override;

		int event(std::string_view id, EventType type, std::size_t index) noexcept override;
		int event(std::string_view id, EventType type) noexcept override;
		int event(std::string_view id, EventType type, std::size_t index, std::string_view value) noexcept override;

};

#endif //CORE
