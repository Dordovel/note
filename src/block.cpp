#include "../data.h"

Block::Block(int index, std::string_view header, std::string_view note, bool is_active)
{
	this->set_header(header);
	this->set_note(note);
	this->set_active(is_active);
}

void Block::set_header(std::string_view header)
{
	this->_header = header;
}

void Block::set_note(std::string_view note)
{
	this->_note = note;
}

void Block::set_active(bool active) noexcept
{
	this->_isActive = active;
}

bool Block::is_active() const noexcept
{
	return this->_isActive;
}

std::string	Block::get_header() const noexcept
{
	return this->_header;
}

std::string	Block::get_note() const noexcept
{
	return this->_note;
}
