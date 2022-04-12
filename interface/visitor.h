#ifndef ISAVE_VISITOR
#define ISAVE_VISITOR

#include <memory>

class IPage;
class Line;

class ISavePageVisitor
{
	public:
		virtual void save_page(std::shared_ptr<IPage> page) = 0;
		virtual unsigned int save(int page_number, const Line& line) = 0;
		virtual ~ISavePageVisitor() = default;
};

#endif //ISAVE_VISITOR

