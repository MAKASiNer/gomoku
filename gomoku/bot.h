#pragma once
#include <array>
#include <map>
#include <set>

#include "context.hpp"


class Bot
{
public:
	using container_t = std::set<coord_t>;
	using pattern_t = std::vector<int>;
	using pattern_container_t = std::map<pattern_t, float>;

	Bot();
	Bot(Context* _context);

	coord_t move() const;

protected:
	Context* context;
	static const pattern_container_t patterns;
};