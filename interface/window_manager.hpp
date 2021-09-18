#ifndef IWINDOW_MANAGER
#define IWINDOW_MANAGER

#include "window.h"

#include <memory>

enum class WindowType;

class IWindowRegister
{
	public:
		virtual void register_window(std::string_view id, WindowType type, std::shared_ptr<IWindow>&& window) noexcept = 0;

		virtual ~IWindowRegister() = default;
};

class IWindowRegisterGet
{
	public:
		virtual std::weak_ptr<IWindow> get_window(WindowType type) noexcept = 0;
		virtual std::weak_ptr<IWindow> get_window(std::string_view id) noexcept = 0;

		virtual ~IWindowRegisterGet() = default;
};

#endif //IWINDOW_MANAGER	
