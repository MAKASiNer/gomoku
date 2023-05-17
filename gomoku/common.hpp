#pragma once
#include <utility>


using cell_t = char;
using coord_t = std::pair<int, int>;

enum class SceneID {
	init,
	home,
	settings,
	play,
	wins,
	exit
};