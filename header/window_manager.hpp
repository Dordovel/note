#ifndef WINDOW_MANAGER
#define WINDOW_MANAGER

#include "../interface/window_manager.hpp"

#include <unordered_map>

class WindowManager : public IWindowRegister, public IWindowRegisterGet
{
	private:
		std::unordered_map<std::string, std::shared_ptr<IWindow>>  _registeredWindows;
		std::unordered_map<WindowType, decltype(_registeredWindows)::iterator> _registeredWindowsPointer;
	public:
		void register_window(std::string_view id, WindowType type, std::shared_ptr<IWindow>&& window) noexcept override;
		std::weak_ptr<IWindow> get_window(WindowType type) noexcept override;
		std::weak_ptr<IWindow> get_window(std::string_view id) noexcept override;
};


#endif //WINDOW_MANAGER	
