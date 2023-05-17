#pragma once
#include "board.h"


class Context
{
public:
	Board board;
	std::vector<coord_t> moves;
	int board_width = Board::min_width;
	int board_height = Board::min_height;

	// ������� ����� ����� � ������� Context::board_width Context:: board_height � ������� ������� �����
	void new_board()
	{
		board = Board(board_width, board_height);
		moves.clear();
	}


	bool vs_computer = true;
	cell_t bot_side = Board::O;
	cell_t current_player = Board::X;

	// true ���� ������ ����� �� �������
	bool bot_move() { return vs_computer && (bot_side == current_player); }

	// true ���� ������ ����� �� ���
	bool human_move() { return !bot_move(); }

	// true ���� ������ ��� ���������
	bool x_move() { return current_player == Board::X; }

	// true ���� ������ ��� �������
	bool o_move() { return current_player == Board::O; }

	// �������� ��� ���������� ������
	void pass_move()
	{
		current_player =
			(current_player == Board::X) ? Board::O :
			(current_player == Board::O) ? Board::X :
			Board::_;
	};

	// �������� ��� ���������
	void pass_move_to_x() { current_player = Board::X; }

	// �������� ��� �������
	void pass_move_to_o() { current_player = Board::O; }


	cell_t winner = Board::_;

	// ������� ���
	bool bot_wins() { return vs_computer && winner == bot_side; }

	// ������� 1 �����
	bool pl1_wins() { return !bot_wins() && winner == Board::X; }

	// ������� 2 �����
	bool pl2_wins() { return !bot_wins() && winner == Board::O; }


	// ������ ��� � �������� � ��� � ����� ����� � �������� ��� ���������� ������
	bool make_move(coord_t pos)
	{
		if (board.inside(pos.first, pos.second))
		{
			if (board.at(pos.first, pos.second) == Board::_)
			{
				board.at(pos.first, pos.second) = current_player;
				moves.push_back(pos);
				pass_move();
				return true;
			}
		}
		return false;
	}


	coord_t player_click_on_board{ -1, -1 };

	// ������� ��������� ���� �� �������� ����
	void clear_click_on_board() { player_click_on_board = { -1, -1 }; }

	
	int screen_width = 800;
	int screen_height = 600;

	static const size_t plname_size = 40;
	char pl1name[plname_size]{ "Player1" };
	char pl2name[plname_size]{ "Player2" };

	double time;

	SceneID scene = SceneID::init;
	
};