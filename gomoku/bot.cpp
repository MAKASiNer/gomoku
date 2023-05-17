#include "bot.h"


const Bot::pattern_container_t Bot::patterns{
	{{1, 1, 1, 1, 1}, INFINITY},
	{{0, 1, 1, 1, 1, 0},  7000},
	{{0, 1, 1, 1, 1},     4000},
	{{1, 1, 1, 1, 0},     4000},
	{{0, 1, 1, 1, 0},     3000},
	{{0, 1, 0, 1, 1, 1},  2000},
	{{0, 1, 1, 0, 1, 1},  2000},
	{{0, 1, 1, 1, 0, 1},  2000},
	{{1, 1, 1, 0, 1, 0},  2000},
	{{1, 1, 0, 1, 1, 0},  2000},
	{{1, 0, 1, 1, 1, 0},  2000},
	{{0, 1, 1, 1},        1500},
	{{1, 1, 1, 0},        1500},
	{{0, 1, 1, 0, 1},      800},
	{{0, 1, 0, 1, 1},      800},
	{{1, 1, 0, 1, 1},      800},
	{{1, 0, 1, 1, 0},      800},
	{{0, 1, 1, 0},         200},
};



Bot::Bot()
	: Bot(nullptr) {}


Bot::Bot(Context* _context)
	: context{ _context } {}


coord_t Bot::move() const
{
	using std::find;
	using std::search;
	using std::distance;

	// ���� ��� ������������� ����
	container_t moves;
	for (int i = 0; i < context->board.container.size(); i++)
	{
		if (context->board.container[i] == Board::_) continue;

		// ������������� ���� ���� ��� ������ � �������� 5�5 ������ �������� ������
		for (int dx = -2; dx < 3; dx++)
		{
			for (int dy = -2; dy < 3; dy++)
			{
				int x = context->board.i2x(i) + dx;
				int y = context->board.i2y(i) + dy;

				if (!context->board.inside(x, y) || context->board.at(x, y) != Board::_) continue;
				moves.insert({ x, y });
			}
		}
	}

	// ������ ��� �� ��������� � ������ (�� ������ ����� ���� ������)
	coord_t best_move{ context->board.width / 2, context->board.height / 2 };
	float best_move_w = 0;

	// ��� ������� ���� ����� 4 ����� ������� � 9 �.�.(n*2-1)
	for (auto& move : moves)
	{
		using line_t = std::array<int, 9>;

		std::array<line_t, 4> attack_lines;
		std::array<line_t, 4> defence_lines;
		std::array<coord_t, 4> dirs{ coord_t{1, 0},coord_t{0, 1}, coord_t{1, 1}, coord_t{1, -1} };

		// ���������� �� ������ ����� (��������������, ������������, �������� ���������, �������� ���������)
		for (int i = 0; i < 4; i++)
		{
			coord_t& dir = dirs[i];
			line_t& attack_line = attack_lines[i];
			line_t& defence_line = defence_lines[i];

			for (int k = -4; k < 5; k++)
			{
				int x = move.first + k * dir.first;
				int y = move.second + k * dir.second;
				if (!context->board.inside(x, y)) continue;
				cell_t cell = context->board.at(x, y);
				// � ������ �����: 0 - �����, +1 - ���� ������, -1 - ������ �����������
				// � ������������: 0 - �����, -1 - ���� ������, +1 - ������ �����������
				if (cell == Board::_)
				{
					attack_line[k + 4] = 0;
					defence_line[k + 4] = 0;
				}
				else if (cell == context->bot_side)
				{
					attack_line[k + 4] = 1;
					defence_line[k + 4] = -1;
				}
				else
				{
					attack_line[k + 4] = -1;
					defence_line[k + 4] = 1;
				}
			}
		}

		// ��������� ��������� ��� ��� ����������� �����
		float move_w = 1 + rand() % 30;
		float fork = 0;

		// ���� ��������� ������ ����� � ������ ��������
		for (auto& lines : { defence_lines, attack_lines })
		{
			for (const auto& line : lines)
			{
				for (const auto& kv : patterns)
				{
					const pattern_t& pattern = kv.first;
					float w = kv.second;

					auto it = search(line.begin(), line.end(), pattern.begin(), pattern.end());
					if (it == line.end()) continue;
					// ����� ����� - ������, ��� ������� ��������� �����
					// ��� �� ���������� ������, ������� �������� � ���� ����������� ������ �����
					size_t start = distance(line.begin(), it);
					if (!(start <= 4 && 4 < start + pattern.size())) continue;
					// ���� ������ ������ ���������� ������� "�����" � ��� ���� 
					move_w += w;
					fork += 1;
					// ������� ������������� �� �� �����������
					// ������ ������ �������� ������ � ����� � ������������ ����� ��� ���� �����
					break;
				}
			}

			// ������� ����� ����������� ��� ����
			if (fork > 1) move_w *= 2;

			// � ������� ������� ����
			if (move_w > best_move_w)
			{
				best_move = move;
				best_move_w = move_w;
			}
		}
	}

	return best_move;
}