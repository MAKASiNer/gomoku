#pragma once
#include "board.h"


class Context
{
public:
	Board board;
	std::vector<coord_t> moves;
	int board_width = Board::min_width;
	int board_height = Board::min_height;

	// создает новую доску с размера Context::board_width Context:: board_height и стирает историю ходов
	void new_board()
	{
		board = Board(board_width, board_height);
		moves.clear();
	}


	bool vs_computer = true;
	cell_t bot_side = Board::O;
	cell_t current_player = Board::X;

	// true если сейчас ходит не человек
	bool bot_move() { return vs_computer && (bot_side == current_player); }

	// true если сейчас ходит не бот
	bool human_move() { return !bot_move(); }

	// true если сейчас ход крестиков
	bool x_move() { return current_player == Board::X; }

	// true если сейчас ход ноликов
	bool o_move() { return current_player == Board::O; }

	// передает ход следующему игроку
	void pass_move()
	{
		current_player =
			(current_player == Board::X) ? Board::O :
			(current_player == Board::O) ? Board::X :
			Board::_;
	};

	// передает ход крестикам
	void pass_move_to_x() { current_player = Board::X; }

	// передает ход ноликам
	void pass_move_to_o() { current_player = Board::O; }


	cell_t winner = Board::_;

	// победил бот
	bool bot_wins() { return vs_computer && winner == bot_side; }

	// победил 1 игрок
	bool pl1_wins() { return !bot_wins() && winner == Board::X; }

	// победил 2 игрок
	bool pl2_wins() { return !bot_wins() && winner == Board::O; }


	// делает ход и помещает в его в буфер ходов и передает ход следующему игроку
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

	// очищает последний клик по игровому полю
	void clear_click_on_board() { player_click_on_board = { -1, -1 }; }

	
	int screen_width = 800;
	int screen_height = 600;

	static const size_t plname_size = 40;
	char pl1name[plname_size]{ "Player1" };
	char pl2name[plname_size]{ "Player2" };

	double time;

	SceneID scene = SceneID::init;
	
};