#ifndef IVISITOR
#define IVISITOR

#include "visitor.h"

class ISaveVisit
{
	public:
		virtual void save_visitor(ISavePageVisitor* visitor) = 0;
		virtual ~ISaveVisit() = default;
};

#endif //IVISITOR
