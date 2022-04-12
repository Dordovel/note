#ifndef PAGE
#define PAGE

#include "line.h"
#include "../interface/database.h"
#include "../interface/page.h"
#include "../interface/visitor.h"
#include "../interface/visit.h"
#include <memory>
#include <vector>

class Page : public IPage
{
	private:
		std::vector<Line> _lines;
		int page_number;
	public:
		Page(int number);
		Page(int number, std::vector<Line>&& lines);
		Page(int number, const std::vector<Line>& lines);

		Block& insert_line() noexcept override;
		void insert_line(const Block& line) noexcept override;
		void insert_line(Block&& line) noexcept override;

		void replace_line(std::size_t index, const Block& line) override;
		void replace_line(std::size_t index, Block&& line) override;

		bool is_change() const noexcept override;

		Block& get_line_by_id(std::size_t index) noexcept override;
		std::size_t get_line_count() const noexcept override;
		void delete_line_by_id(std::size_t index) noexcept override;

		void reset_blocks() noexcept override;

		void save_visitor(ISavePageVisitor* visitor) override;

		~Page() override = default;
};

#endif //PAGE
