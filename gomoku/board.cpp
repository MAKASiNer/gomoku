#include "board.h"


const cell_t Board::X = 'x';
const cell_t Board::O = '0';
const cell_t Board::_ = '.';
const size_t Board::max_width = 20;
const size_t Board::min_width = 10;
const size_t Board::max_height = 20;
const size_t Board::min_height = 10;


Board::Board()
	: Board(0, 0) {};


Board::Board(const Board& other)
	: _width{ other.width }, _height{ other.height }, _container{ other.container } {}


Board::Board(size_t w, size_t h)
	: _width(clamp_w(w)), _height(clamp_h(h)), _container(clamp_w(w)* clamp_h(h), _) {}


Board& Board::operator=(const Board& other)
{
	_width = other.width;
	_height = other.height,
		_container = other.container;
	return *this;
}


void Board::fill(cell_t val)
{
	return std::fill(_container.begin(), _container.end(), val);
}


const cell_t& Board::at(size_t x, size_t y) const
{
	return _container[xy2i(x, y)];
}


cell_t& Board::at(size_t x, size_t y)
{
	return _container[xy2i(x, y)];
}


size_t Board::xy2i(size_t x, size_t y) const
{
	return x + y * width;
}


size_t Board::i2x(size_t i) const
{
	return i % width;
}


size_t Board::i2y(size_t i) const
{
	return i / width;
}


bool Board::inside(size_t x, size_t y) const
{
	return x < width&& y < height;
}


size_t Board::clamp_w(size_t w) const
{
	return std::clamp(w, min_width, max_width);
}


size_t Board::clamp_h(size_t h) const
{
	return std::clamp(h, min_height, max_height);
}


void Board::save(std::string filename) const
{
	return save(std::wstring(filename.begin(), filename.end()));
}


void Board::save(std::wstring filename) const
{
	std::ofstream file(filename, std::ios::binary);
	file.write((char*)&width, sizeof(width));
	file.write((char*)&height, sizeof(height));
	for (const cell_t& cell : container) file.write((char*)&cell, sizeof(cell));
	file.close();
}


void Board::load(std::string filename)
{
	return load(std::wstring(filename.begin(), filename.end()));
}


void Board::load(std::wstring filename)
{
	std::ifstream file(filename, std::ios::binary);
	file.read((char*)&width, sizeof(width));
	file.read((char*)&height, sizeof(height));
	_container.resize(_width * _height, Board::_);
	for (cell_t& cell : _container) file.read((char*)&cell, sizeof(cell));
	file.close();
}


cell_t Board::check_win() const
{
	int n = 5;

	// горизонтальный ряд ровно из n
	for (int j = 0; j < height; j++)
	{
		int tics = 0;
		int tacs = 0;

		for (int i = 0; i < width; i++)
		{
			const cell_t& cell = at(i, j);

			if (cell == Board::X)
			{
				tics++;
				if (tacs == n) break;
				else tacs = 0;
			}
			if (cell == Board::O)
			{
				tacs++;
				if (tics == n) break;
				else tics = 0;
			}
			if (cell == Board::_)
			{
				if (tics == n) break;
				else tics = 0;

				if (tacs == n) break;
				else tacs = 0;
			}
		}
		if (tics == n) return Board::X;
		if (tacs == n) return Board::O;
	}

	// вертикальный ряд ровно из n
	for (int i = 0; i < width; i++)
	{
		int tics = 0, tacs = 0;
		for (int j = 0; j < height; j++)
		{
			const cell_t& cell = at(i, j);
			if (cell == Board::X) { tics++; if (tacs == n) break; else tacs = 0; }
			if (cell == Board::O) { tacs++; if (tics == n) break; else tics = 0; }
			if (cell == Board::_) { if (tics == n) break; else tics = 0; if (tacs == n) break; else tacs = 0; }
		}
		if (tics == n) return Board::X;
		if (tacs == n) return Board::O;
	}

	// ряд ровно из n параллельный оснровной диагонали
	for (int i = 0; i < width - n; i++)
	{
		int tics = 0, tacs = 0;
		for (size_t k = 0; i + k < width && k < height; k++)
		{
			const cell_t& cell = at(i + k, k);
			if (cell == Board::X) { tics++; if (tacs == n) break; else tacs = 0; }
			if (cell == Board::O) { tacs++; if (tics == n) break; else tics = 0; }
			if (cell == Board::_) { if (tics == n) break; else tics = 0; if (tacs == n) break; else tacs = 0; }
		}
		if (tics == n) return Board::X;
		if (tacs == n) return Board::O;
	}

	for (int j = 0; j < height - n; j++)
	{
		int tics = 0, tacs = 0;
		for (size_t k = 0; k < width && j + k < height; k++)
		{
			const cell_t& cell = at(k, j + k);
			if (cell == Board::X) { tics++; if (tacs == n) break; else tacs = 0; }
			if (cell == Board::O) { tacs++; if (tics == n) break; else tics = 0; }
			if (cell == Board::_) { if (tics == n) break; else tics = 0; if (tacs == n) break; else tacs = 0; }
		}
		if (tics == n) return Board::X;
		if (tacs == n) return Board::O;
	}

	// ряд ровно из n параллельный побочной диагонали
	for (int i = 0; i < width - n; i++)
	{
		int tics = 0, tacs = 0;
		for (size_t k = 0; i + k < width && k < height; k++)
		{
			const cell_t& cell = at(i + k, height - k - 1);
			if (cell == Board::X) { tics++; if (tacs == n) break; else tacs = 0; }
			if (cell == Board::O) { tacs++; if (tics == n) break; else tics = 0; }
			if (cell == Board::_) { if (tics == n) break; else tics = 0; if (tacs == n) break; else tacs = 0; }
		}
		if (tics == n) return Board::X;
		if (tacs == n) return Board::O;
	}

	for (int j = 0; j < height - n; j++)
	{
		int tics = 0, tacs = 0;
		for (size_t k = 0; k < width && j + k < height; k++)
		{
			const cell_t& cell = at(width - k - 1, j + k);
			if (cell == Board::X) { tics++; if (tacs == n) break; else tacs = 0; }
			if (cell == Board::O) { tacs++; if (tics == n) break; else tics = 0; }
			if (cell == Board::_) { if (tics == n) break; else tics = 0; if (tacs == n) break; else tacs = 0; }
		}
		if (tics == n) return Board::X;
		if (tacs == n) return Board::O;
	}


	return Board::_;
};