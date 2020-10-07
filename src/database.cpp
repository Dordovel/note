#include "../header/database.h"

#include "../data.h"

#include <sqlite3.h>
#include <string>
#include <algorithm>


namespace
{
	std::string concat(const std::vector<std::string>& columns, std::string_view separate = ", ")
	{
		std::string buffer;
		auto lastColumn = std::prev(std::end(columns));

		std::for_each(std::begin(columns), lastColumn, [&buffer, separate](std::string column){buffer += column + separate.data();});
		
		buffer += *(lastColumn);

		return buffer;
	}
};

int Database::connect(std::string_view path) noexcept 
{
	return sqlite3_open(path.data(), &this->db);
}

int Database::query_insert(std::string_view table, const std::vector<std::string>& columns, const std::vector<std::string>& data) noexcept
{
	std::string query(std::string("INSERT INTO ") + table.data());

	query += " (" + concat(columns) + ")";

	query += " VALUES ('" + concat(data, "', '") + "')";

	return sqlite3_exec(this->db, query.c_str(), nullptr, nullptr, nullptr);
}

int Database::query_update(std::string_view table, const std::vector<std::string>& columns, const std::vector<std::string>& data, const std::vector<std::string> predicate) noexcept
{
	std::string query(std::string("UPDATE ") + table.data());

	const std::size_t size = columns.size();

	if(size != data.size())
		return -1;

	std::vector<std::string> buffer;
	buffer.reserve(size);

	for(std::size_t a = 0; a < size; ++a)
	{
		buffer.emplace_back(columns.at(a) + " = " + data.at(a));
	}

	query += " SET " + concat(buffer);

	if(!predicate.empty())
	{
		query += " WHERE " + concat(predicate, " AND ");
	}

	return sqlite3_exec(this->db, query.c_str(), nullptr, nullptr, nullptr);
}

int Database::query_delete(std::string_view table, const std::vector<std::string>& columns, const std::vector<std::string>& predicate) noexcept
{
	std::string query(std::string("DELETE FROM ") + table.data());

	if(!predicate.empty())
	{
		query += " WHERE " + concat(predicate, " AND ");
	}

	return sqlite3_exec(this->db, query.c_str(), nullptr, nullptr, nullptr);
}

std::vector<std::vector<std::string>> Database::query_select(std::string_view table, const std::vector<std::string>& columns, const std::vector<std::string>& predicate) noexcept
{
	std::string query(std::string("SELECT "));

	if(!columns.empty())
		query += concat(columns);
	else
		query += " *";

	query += std::string(" FROM ") + table.data();	

	if(!predicate.empty())
	{
		query += " WHERE " + concat(predicate, " AND ");
	}

	sqlite3_stmt* stmt;
	sqlite3_exec(this->db, query.c_str(), nullptr, nullptr, nullptr);
	sqlite3_prepare_v2(this->db, query.c_str(), query.size(), &stmt, nullptr);
	
	int columnCount = 0;

	std::vector<std::vector<std::string>> result;
	std::vector<std::string> buffer;
	buffer.reserve(4);

	while(sqlite3_step(stmt) == SQLITE_ROW)
	{
		columnCount = sqlite3_column_count(stmt);

		for(int a = 0; a < columnCount; ++a)
		{
			buffer.emplace_back((char*)sqlite3_column_text(stmt, a));
		}
		result.emplace_back(std::move(buffer));
	}

	return result;
}

