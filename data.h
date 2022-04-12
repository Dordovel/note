#ifndef DATA_STRUCT
#define DATA_STRUCT

#include <string>

class Block
{
    std::string _header;
    std::string _note;
    bool _isActive;

	public:
		Block() = default;
		Block(int index, std::string_view header, std::string_view note, bool is_active);
		Block(Block&&) = default;
		Block(const Block&) = default;
		Block& operator=(const Block&) = default;
		Block& operator=(Block&&) = default;
		~Block() = default;

		bool is_active() const noexcept;
		std::string get_header() const noexcept;
		std::string get_note() const noexcept;

		virtual void set_header(std::string_view header);
		virtual void set_note(std::string_view note);
		virtual void set_active(bool active) noexcept;
};

#endif //DATA_STRUCT
