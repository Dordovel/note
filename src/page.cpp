#include "../header/page.h"
#include "../interface/page.h"

#include <algorithm>
#include <cstddef>
#include <numeric>

auto line_great_comparator = [](Line lv, Line rv) {
		return lv.get_index() > rv.get_index();
	 };

Page::Page(int number)
{
	this->page_number = number;
}

Page::Page(int number, std::vector<Line>&& lines)
{
	this->page_number = number;
	this->_lines = std::move(lines);
}

Page::Page(int number, const std::vector<Line>& lines)
{
	this->page_number = number;
	this->_lines = lines;
}

void Page::insert_line(const Block& line) noexcept
{
	auto lastRow = std::max_element(std::begin(this->_lines), std::end(this->_lines), line_great_comparator);

	auto index = lastRow != std::end(this->_lines)? lastRow->get_index() : 0;
	Line newLine = Line(index + 1, line);

	newLine.set_status(LineStatus::NONE);
	newLine.set_created_status(LineCreatedStatus::CREATED);

	this->_lines.emplace_back(std::move(newLine));
}

void Page::insert_line(Block&& line) noexcept
{
	auto lastRow = std::max_element(std::begin(this->_lines), std::end(this->_lines), line_great_comparator);
	
	auto index = lastRow != std::end(this->_lines)? lastRow->get_index() : 0;

	Line newLine = Line(index + 1, std::move(line));
	newLine.set_status(LineStatus::NONE);
	newLine.set_created_status(LineCreatedStatus::CREATED);

	this->_lines.emplace_back(std::move(newLine));
}

Block& Page::insert_line() noexcept
{
	auto lastRow = std::max_element(std::begin(this->_lines), std::end(this->_lines), line_great_comparator);

	auto index = lastRow != std::end(this->_lines)? lastRow->get_index() : 0;

	return this->_lines.emplace_back(Line(index + 1));
}

void Page::replace_line(std::size_t index, const Block& line)
{
	decltype(this->_lines)::size_type size = this->get_line_count();

	if(index >= 0 && index < size)
	{
		Line& replaced = this->_lines.at(index);
		replaced.set_header(line.get_header());
		replaced.set_note(line.get_note());
		replaced.set_active(line.is_active());
	}
}

void Page::replace_line(std::size_t index, Block&& line)
{
	decltype(this->_lines)::size_type size = this->get_line_count();

	if(index >= 0 && index < size)
	{
		Line& replaced = this->_lines.at(index);
		replaced.set_header(line.get_header());
		replaced.set_note(line.get_note());
		replaced.set_active(line.is_active());
	}
}

bool Page::is_change() const noexcept
{
	for(const Line& line : this->_lines)
	{
		if(line.line_is_change())
		{
			return true;
		}
	}

	return false;
}

Block& Page::get_line_by_id(std::size_t index) noexcept
{
	return this->_lines.at(index);
}

std::size_t Page::get_line_count() const noexcept
{
	return std::count_if(std::begin(this->_lines), std::end(this->_lines), [](Line lv) {
		return lv.get_status() != LineStatus::DELETE;
	});
}

void Page::delete_line_by_id(std::size_t index) noexcept
{
	decltype(this->_lines)::size_type size = this->get_line_count();

	if(index >= 0 && index < size)
	{
		auto iterator = std::next(std::begin(this->_lines), index);
		iterator->set_status(LineStatus::DELETE);

		decltype(this->_lines)::iterator lv(iterator), rv(iterator);
		while (( ++rv ) != this->_lines.end())
		{
			std::swap(*( lv++ ), *rv);
		}
	}
}

void Page::save_visitor(ISavePageVisitor* visitor)
{
	for(Line& line : this->_lines)
	{
		line.set_index(visitor->save(this->page_number, line));
	}
}

void Page::reset_blocks() noexcept
{
	for(auto& line : this->_lines)
	{
		line.set_status(LineStatus::NONE);
		line.set_created_status(LineCreatedStatus::NONE);
	}
}
