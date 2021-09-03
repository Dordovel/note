#ifndef DATABASE
#define DATABASE

#include "../interface/database.h"
#include <unordered_map>

struct sqlite3;

class Database final : public IDatabase
{ private: sqlite3* db;
	public:
		int connect(std::string_view path) noexcept override;
		int last_insert_id() const noexcept override;
		int query_insert(std::string_view table, const std::vector<std::string>& columns, const std::vector<std::string>& data) noexcept override;
		int query_update(std::string_view table, const std::vector<std::string>& columns, const std::vector<std::string>& data, const std::vector<std::string> predicate) noexcept override;
		int query_delete(std::string_view table, const std::vector<std::string>& predicate) noexcept override;
		std::vector<std::unordered_map<std::string, std::string>> query_select(std::string_view table, const std::vector<std::string>& columns, const std::vector<std::string>& predicate) noexcept override;
};

#endif //DATABASE
