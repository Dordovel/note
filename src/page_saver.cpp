#include <iostream>
#include "../header/page_saver.h"
#include "../header/line.h"

PageSaver::PageSaver(std::shared_ptr<IDatabase> database)
{
	this->_database = std::move(database);
}

void PageSaver::save_page(std::shared_ptr<IPage> page)
{
	page->save_visitor(this);
}

unsigned int PageSaver::save(int page_number, const Line& line)
{
	if(line.get_created_status() == LineCreatedStatus::CREATED)
	{
		this->_database->query_insert(this->_table,
				{"parent", "data", "status", "note"},
				{ std::to_string(line.get_index()), line.get_header(), std::to_string(line.is_active()), line.get_note()});

		return this->_database->last_insert_id();
	}

	if(line.get_created_status() == LineCreatedStatus::NONE)
	{
		switch(line.get_status())
		{
			case LineStatus::CHANGE:
			{
				this->_database->query_update(this->_table,
						{"data", "status", "note"},
						{line.get_header(), std::to_string(line.is_active()), line.get_note()},
						{"id = " + std::to_string(line.get_index()), "parent = " + std::to_string(page_number)});
			}
			break;

			case LineStatus::DELETE:
			{
				this->_database->query_update(this->_table,
						{"deleted"},
						{"1"},
						{"id = " + std::to_string(line.get_index()), "parent = " + std::to_string(page_number)});

				this->_database->query_update(this->_table,
						{"deleted"},
						{"1"},
						{"parent = " + std::to_string(line.get_index())});
			}
			break;

			default: break;
		}
	}

	return 0;
}
