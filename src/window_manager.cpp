#include "../header/window_manager.hpp"
#include <memory>

std::weak_ptr<IWindow> WindowManager::get_window(std::string_view id) noexcept
{
	return this->_registeredWindows[id.data()];
}

std::weak_ptr<IWindow> WindowManager::get_window(WindowType type) noexcept
{
	return this->_registeredWindowsPointer[type]->second;
}

void WindowManager::register_window(std::string_view id, WindowType type, std::shared_ptr<IWindow>&& window) noexcept
{
	auto iterator = this->_registeredWindows.emplace(id, std::move(window));
	this->_registeredWindowsPointer.emplace(type, iterator.first);
}
