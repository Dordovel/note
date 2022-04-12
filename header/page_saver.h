#ifndef IPAGE_SAVE
#define IPAGE_SAVE

#include "../interface/database.h"
#include "../interface/page.h"
#include "../interface/visitor.h"

#include <string>
#include <memory>

class PageSaver : public ISavePageVisitor
{
	private:
		std::shared_ptr<IDatabase> _database;
		static const inline std::string _table = "main";
	public:
		explicit PageSaver(std::shared_ptr<IDatabase> database);

		unsigned int save(int page_number, const Line& line) override;

		void save_page(std::shared_ptr<IPage> page) override;
};

#endif //IPAGE_SAVE
