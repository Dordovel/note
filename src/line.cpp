#include "../header/line.h"

Line::Line(int index, std::string_view header, std::string_view note, bool is_active): Block(index, header, note, is_active)
{
	this->_lineIndex = index;
}

Line::Line(int index)
{
	this->_lineIndex = index;
}

Line::Line(int index, const Block& block):Block(block)
{
	this->_lineIndex = index;
}

Line::Line(int index, Block&& block):Block(block)
{
	this->_lineIndex = index;
}

LineStatus Line::get_status() const noexcept
{
	return this->_status;
}

LineCreatedStatus Line::get_created_status() const noexcept
{
	return this->_created;
}

void Line::set_status(LineStatus status) noexcept
{
	this->_status = status;
}

void Line::set_created_status(LineCreatedStatus created) noexcept
{
	this->_created = created;
}

int Line::get_index() const noexcept
{
	return this->_lineIndex;
}

void Line::set_index(int index) noexcept
{
	this->_lineIndex = index;
}

bool Line::line_is_change() const noexcept
{
	if(this->_created == LineCreatedStatus::NONE)
	{
		if(this->_status != LineStatus::NONE) return true;
	}
	else if (this->_created == LineCreatedStatus::CREATED)
	{
		if(this->_status != LineStatus::NONE
				&& this->_status != LineStatus::DELETE)
		{
			return false;
		}
	}
	return false;
}

void Line::set_header(std::string_view header)
{
	std::string oldHeader = Block::get_header();
	if(oldHeader != header)
	{
		Block::set_header(header);
		this->_status = LineStatus::CHANGE;
	}
}

void Line::set_note(std::string_view note)
{
	std::string oldNote = Block::get_note();
	if(oldNote != note)
	{
		Block::set_note(note);
		this->_status = LineStatus::CHANGE;
	}
}

void Line::set_active(bool active) noexcept
{
	if(Block::is_active() != active)
	{
		Block::set_active(active);
		this->_status = LineStatus::CHANGE;
	}
}
