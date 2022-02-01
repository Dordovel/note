#ifndef COMPONENT_CPP
#define COMPONENT_CPP

#include "../data.h"

class IComponent
{
	public:
		virtual int get_selected_item() const noexcept = 0;
		virtual void add(const Data& value) noexcept = 0;
		virtual void clear() noexcept = 0;

		virtual ~IComponent() = default;
};

#endif //COMPONENT_CPP
