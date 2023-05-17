#pragma once
#include <algorithm>
#include <fstream>
#include <vector>

#include "common.hpp"


class Board
{
public:
	using container_t = std::vector<cell_t>;

	const container_t& container = _container;
	const size_t& width = _width;
	const size_t& height = _height;

	static const cell_t X;
	static const cell_t O;
	static const cell_t _;

	static const size_t max_width;
	static const size_t min_width;
	static const size_t max_height;
	static const size_t min_height;

	Board();
	Board(const Board& other);
	Board(size_t w, size_t h);

	Board& operator=(const Board& other);

	void fill(cell_t val);

	const cell_t& at(size_t x, size_t y) const;
	cell_t& at(size_t x, size_t y);

	size_t xy2i(size_t x, size_t y) const;
	size_t i2x(size_t i) const;
	size_t i2y(size_t i) const;
	size_t clamp_w(size_t w) const;
	size_t clamp_h(size_t h) const;

	bool inside(size_t x, size_t y) const;

	void save(std::string filename) const;
	void save(std::wstring filename) const;

	void load(std::string filename);
	void load(std::wstring filename);

	cell_t check_win() const;

protected:
	container_t _container;
	size_t _width;
	size_t _height;
};
