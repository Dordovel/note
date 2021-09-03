#ifndef DATABASE_INTERFACE
#define DATABASE_INTERFACE

#include <string_view>
#include <vector>
#include <unordered_map>


class IDatabase
{
	public:
		virtual int connect(std::string_view path) noexcept = 0;
		virtual int last_insert_id() const noexcept = 0;
		virtual int query_insert(std::string_view table, const std::vector<std::string>& columns, const std::vector<std::string>& data) noexcept = 0;
		virtual int query_update(std::string_view table, const std::vector<std::string>& columns, const std::vector<std::string>& data, const std::vector<std::string> predicate) noexcept = 0;
		virtual int query_delete(std::string_view table, const std::vector<std::string>& predicate) noexcept = 0;
		virtual std::vector<std::unordered_map<std::string, std::string>> query_select(std::string_view table, const std::vector<std::string>& columns, const std::vector<std::string>& predicate) noexcept = 0;

		virtual ~IDatabase() = default;
};

#endif //DATABASE_INTERFACE
