#include "../header/page_factory.h"

PageFactory::PageFactory(std::shared_ptr<IDatabase> database)
{
	this->_database = std::move(database);
}

std::unique_ptr<IPage> PageFactory::make_page(int page_number)
{
	std::vector<std::string> columns {"id", "data", "status", "parent", "note"};
	std::vector<std::string> predicate {"deleted = 0", std::string("parent = ") + std::to_string(page_number) };

	std::vector<std::unordered_map<std::string, std::string>> result = this->_database->query_select(this->_table, columns, predicate);

	std::vector<Line> buffer;
	buffer.reserve(result.size());

	for(const auto& row : result)
	{
		int id = std::stoi(row.at("id"));
		bool is_active = std::stoi(row.at("status"));
		std::string header = row.at("data");
		std::string note = row.at("note");

		Line& line = buffer.emplace_back(id, header, note, is_active);
		line.set_created_status(LineCreatedStatus::NONE);
		line.set_status(LineStatus::NONE);
	}

	return std::make_unique<Page>(page_number, std::move(buffer));
}

std::unique_ptr<IPage> PageFactory::make_page()
{
	//select MAX(parent) AS max from main WHERE deleted <> 1;
	std::vector<std::string> columns {"MAX(parent) + 1 AS max"};
	std::vector<std::string> predicate {"deleted <> 1"};

	std::vector<std::unordered_map<std::string, std::string>> result = this->_database->query_select(this->_table, columns, predicate);
	if(!result.empty())
	{
		auto last = result.back();
		if(!last.empty())
		{
			auto max = std::stoi(last.at("max"));
			return std::make_unique<Page>(max);
		}
	}
	return std::make_unique<Page>(0);
}
