#ifndef IPAGE
#define IPAGE

#include "../interface/database.h"
#include "visit.h"
#include <memory>
#include <vector>

class Block;

class IPage : public ISaveVisit
{
	public:
		virtual Block& insert_line() noexcept = 0;
		virtual void insert_line(const Block& line) noexcept = 0;
		virtual void insert_line(Block&& line) noexcept = 0;
		virtual void replace_line(std::size_t index, const Block& line) = 0;
		virtual void replace_line(std::size_t index, Block&& line) = 0;
		virtual bool is_change() const noexcept = 0;
		virtual Block& get_line_by_id(std::size_t index) noexcept = 0;
		virtual std::size_t get_line_count() const noexcept = 0;
		virtual void delete_line_by_id(std::size_t index) noexcept = 0;
		virtual void reset_blocks() noexcept = 0;
};

#endif //IPAGE
