#ifndef DATABASE_INTERFACE
#define DATABASE_INTERFACE

#include <string_view>
#include <vector>


class IDatabase
{
	public:
		virtual int connect(std::string_view path) noexcept = 0;
		virtual int query_insert(std::string_view table, const std::vector<std::string>& columns, const std::vector<std::string> data) noexcept = 0;
		virtual int query_update(std::string_view table, const std::vector<std::string>& columns, const std::vector<std::string> data, const std::vector<std::string> predicate) noexcept = 0;
		virtual int query_delete(std::string_view table, const std::vector<std::string>& columns, const std::vector<std::string> predicate) noexcept = 0;
		virtual std::vector<std::vector<std::string>> query_select(std::string_view table, const std::vector<std::string>& columns, const std::vector<std::string>& predicate) noexcept = 0;

		virtual ~IDatabase() = default;
};

#endif //DATABASE_INTERFACE
