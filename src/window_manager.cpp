#include "../header/window_manager.hpp"
#include <memory>

std::weak_ptr<IWindow> WindowManager::window(std::string_view id) noexcept
{
	return this->_windows[id.data()];
}

std::weak_ptr<IWindow> WindowManager::window(WindowType type) noexcept
{
	return this->_windows_by_type[type]->second;
}

void WindowManager::register_window(std::string_view id, WindowType type, std::shared_ptr<IWindow>&& window) noexcept
{
	auto iterator = this->_windows.emplace(id, std::move(window));
	this->_windows_by_type.emplace(type, iterator.first);
}
