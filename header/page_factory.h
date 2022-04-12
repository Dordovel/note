#ifndef PAGE_FACTORY
#define PAGE_FACTORY

#include "../interface/database.h"
#include "page.h"
#include <memory>

class PageFactory
{
	private:
		std::shared_ptr<IDatabase> _database;
		static const inline std::string _table = "main";
	public:
		explicit PageFactory(std::shared_ptr<IDatabase> database);
		std::unique_ptr<IPage> make_page(int page_number);
		std::unique_ptr<IPage> make_page();
};

#endif //PAGE_FACTORY
