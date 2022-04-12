#ifndef LINE
#define LINE

#include "../data.h"
#include <string_view>

enum class LineStatus
{
	CHANGE,
	DELETE,
	NONE
};

enum class LineCreatedStatus
{
	NONE,
	CREATED
};

class Line : public Block
{
	private:
		LineStatus _status;
		LineCreatedStatus _created;
		int _lineIndex;

	public:
		Line(int index);
		Line(int index, std::string_view header, std::string_view note, bool is_active);
		Line(int index, const Block& block);
		Line(int index, Block&& block);

		int get_index() const noexcept;
		void set_index(int index) noexcept;
		bool line_is_change() const noexcept;

		LineStatus get_status() const noexcept;
		LineCreatedStatus get_created_status() const noexcept;

		void set_status(LineStatus status) noexcept;
		void set_created_status(LineCreatedStatus) noexcept;

		void set_header(std::string_view header) override;
		void set_note(std::string_view note) override;
		void set_active(bool active) noexcept override;
};

#endif //LINE
