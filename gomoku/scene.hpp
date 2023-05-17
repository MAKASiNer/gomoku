#pragma once

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif // !_CRT_SECURE_NO_WARNINGS

#define RAYGUI_IMPLEMENTATION
#include <string>
#include <raylib.h>
#include <raymath.h>
#include <raygui.h>

#include "context.hpp"


class Scene
{
public:
	Context* context;

	Scene(Context* _context) : context{ _context } {}

	float fsize() const { return GuiGetStyle(DEFAULT, TEXT_SIZE); }
	float fthickness() const { return 0.09 * fsize(); }

	void draw_mdalign_text(const std::string& str, float x, float y, Color clr) { return draw_mdalign_text(str, x, y, fsize(), clr); }
	void draw_mdalign_text(const std::string& str, float x, float y, float fsize,  Color clr) { return DrawText(str.c_str(), x - textlen(str, fsize) / 2, y, fsize, clr); }

	float textlen(const std::string& str) const { return textlen(str, fsize()); }
	float textlen(const std::string& str, float fsize) const { return MeasureText(str.c_str(), fsize); }
	
	Vector2 screen_center() const { return { float(context->screen_width) / 2, float(context->screen_height) / 2 }; }

	virtual void draw()
	{
		BeginDrawing();
		ClearBackground(RAYWHITE);
		draw_frame();
		EndDrawing();
	}

	virtual void draw_frame() = 0;
};


class InitScene : public Scene
{
public:
	InitScene(Context* context) : Scene(context)
	{
		InitWindow(context->screen_width, context->screen_height, "Tic Tac Toe");
		SetTargetFPS(60);
		GuiSetStyle(DEFAULT, TEXT_SIZE, 28);
		
	};

	void draw_frame() override
	{
		context->scene = SceneID::home;
	}
};


class HomeScene : public Scene
{
public:
	HomeScene(Context* context) : Scene(context) {}

	void draw_frame() override
	{
		draw_mdalign_text("Tic Tac Toe", context->screen_width / 2, context->screen_height / 3, 2 * fsize(), DARKGRAY);

		Rectangle r1{
			screen_center().x - 5 * fsize(),
			screen_center().y,
			10 * fsize(),
			2 * fsize(),
		};

		if (GuiButton(r1, "play"))
		{
			context->time = NULL;
			context->scene = SceneID::settings;
			context->winner = Board::_;
		}
			
	}
};


class SettingsScene : public Scene
{
public:
	SettingsScene(Context* context) : Scene(context) {}

	void draw_frame() override
	{
		float x = context->screen_width / 5;
		float y = context->screen_height / 5;

		draw_enter_pl1name(x, y);
		draw_enter_pl2name(x, y + 2 * fsize());
		draw_enter_vscomputer(x, y + 4 * fsize());
		draw_enter_bwidth(x, y + 6 * fsize());
		draw_enter_bheight(x, y + 8 * fsize());

		const char* text = "start";
		Rectangle rect{ context->screen_width / 2 - textlen(text) / 2, y + 12 * fsize(), 4 * fsize() , 2 * fsize()};
		if (GuiButton(rect, text))
		{
			bool a =  context->vs_computer && strlen(context->pl1name);
			bool b = strlen(context->pl1name) && strlen(context->pl2name) && strcmp(context->pl1name, context->pl2name);
			if (a || b)
			{
				context->new_board();
				context->pass_move_to_x();
				context->scene = SceneID::play;
			}
		}
	}

	void draw_enter_pl1name(float x, float y)
	{
		const char* text = "1st Player:";
		Rectangle rect{ x + textlen(text) + fsize(), y, 10 * fsize(), fsize() };
		
		DrawText(text, x, y, fsize(), GetColor(GuiGetStyle(LABEL, TEXT_COLOR_NORMAL)));
		if (GuiTextBox(rect, context->pl1name, context->plname_size, pl1name_textbox_editbode))
			pl1name_textbox_editbode = !pl1name_textbox_editbode;
	}

	void draw_enter_pl2name(float x, float y)
	{
		const char* text = "2st Player:";
		Rectangle rect{ x + textlen(text) + fsize(), y, 10 * fsize(), fsize() };

		if (!context->vs_computer)
		{
			DrawText(text, x, y, fsize(), GetColor(GuiGetStyle(LABEL, TEXT_COLOR_NORMAL)));
			if (GuiTextBox(rect, context->pl2name, context->plname_size, pl2name_textbox_editbode))
				pl2name_textbox_editbode = !pl2name_textbox_editbode;
		}
		else
		{
			DrawText(text, x, y, fsize(), GetColor(GuiGetStyle(LABEL, TEXT_COLOR_DISABLED)));
			GuiTextBox(rect, context->pl2name, context->plname_size, false);
		}
	}

	void draw_enter_vscomputer(float x, float y)
	{
		Rectangle rect{ x, y, fsize(), fsize() }; 
		context->vs_computer = GuiCheckBox(rect, "VS Computer", context->vs_computer);
	}

	void draw_enter_bwidth(float x, float y)
	{
		const char* text = "Field width: ";
		Rectangle rect{ x + textlen(text), y, 4 * fsize(), fsize()};
		if (GuiSpinner(rect, text, &context->board_width, Board::min_width, Board::max_width, bwidth_spinner_editbode))
			bwidth_spinner_editbode = !bwidth_spinner_editbode;
	}

	void draw_enter_bheight(float x, float y)
	{
		const char* text = "Field height: ";
		Rectangle rect{ x + textlen(text), y, 4 * fsize(), fsize() };
		if (GuiSpinner(rect, text, &context->board_height, Board::min_height, Board::max_height, bheight_spinner_editbode))
			bheight_spinner_editbode = !bheight_spinner_editbode;
	}

private:
	bool pl1name_textbox_editbode = false;
	bool pl2name_textbox_editbode = false;

	bool bwidth_spinner_editbode = false;
	bool bheight_spinner_editbode = false;
};


class PlayScene : public Scene
{
public:
	PlayScene(Context* context): Scene(context) {}

	Vector2 board_pos() const
	{
		return {
			(context->board.max_width - context->board.width + 2) * fsize() / 2.f,
			(context->screen_height - context->board.height * fsize()) / 2.f
		};
	}

	void draw_frame() override
	{
		if (context->time == NULL)
			context->time = GetTime();

		draw_timer((context->board.max_width + 2) * fsize(), fsize());
		draw_whose_move((context->board.max_width + 2) * fsize(), 3 * fsize());
		draw_board(board_pos().x, board_pos().y);
		draw_cursor(GetMousePosition().x, GetMousePosition().y);

		if (context->winner != Board::_)
			context->scene = SceneID::wins;

	}

	void draw_timer(float x, float y)
	{
		double t = GetTime() - context->time;
		char mbuf[3]{};
		_itoa((int)(t / 60) % 100, mbuf, 10);
		char sbuf[3]{};
		_itoa((int)t % 60, sbuf, 10);
		char text[6]{};
		strcat(strcat(strcat(text, mbuf), ":"), sbuf);
		DrawText(text, x, y, fsize(), DARKGRAY);
	}

	void draw_whose_move(float x, float y)
	{
		const char* text = "Move: ";
		DrawText(text, x, y, fsize(), DARKGRAY);
		if (context->x_move()) draw_x(x + textlen(text), y);
		if (context->o_move()) draw_o(x + textlen(text), y);
	}

	void draw_board(float x, float y)
	{
		Vector2 bpos = board_pos();

		// крестики и нолики
		for (int j = 0; j < context->board.height; j++)
		{
			for (int i = 0; i < context->board.width; i++)
			{
				float x = i * fsize() + bpos.x;
				float y = j * fsize() + bpos.y;
				if (context->board.at(i, j) == Board::X) draw_x(x, y);
				else if (context->board.at(i, j) == Board::O) draw_o(x, y);
			}
		}

		// горизонтальные линии
		for (int j = 0; j <= context->board.height; j++)
		{
			Vector2 start = Vector2Add({ 0, j * fsize() }, { x, y });
			Vector2 end = Vector2Add({ context->board.width * fsize(), j * fsize() }, { x, y });
			DrawLineV(start, end, GRAY);
		}

		// вертикальные линии
		for (int i = 0; i <= context->board.width; i++)
		{
			Vector2 start = Vector2Add({ i * fsize(), 0 }, { x, y });
			Vector2 end = Vector2Add({ i * fsize(), context->board.height * fsize() }, { x, y });
			DrawLineV(start, end, GRAY);
		}

		// номера ходов
		for (int i = 0; i < context->moves.size(); i++)
		{
			float x = 0 + context->moves[i].first * fsize() + bpos.x;
			float y = 0 + context->moves[i].second * fsize() + bpos.y;
			DrawText(std::to_string(i + 1).c_str(), x, y, fsize() / 8, BLACK);
		}
	}

	void draw_x(float x, float y)
	{
		float a = 0.707f; // sin(pi/4)
		Vector2 o = { x + 0.5 * fsize(), y + fsize() - r };
		DrawLineEx({ o.x - a * r, o.y - a * r }, { o.x + a * r, o.y + a * r }, fthickness(), DARKBLUE);
		DrawLineEx({ o.x + a * r, o.y - a * r }, { o.x - a * r, o.y + a * r }, fthickness(), DARKBLUE);
	}

	void draw_o(float x, float y)
	{
		Vector2 o = { x + 0.5 * fsize(), y + fsize() - r };
		DrawCircleV(o, r - fthickness(), MAROON);
		DrawCircleV(o, r - 2 * fthickness(), RAYWHITE);
	}

	void draw_cursor(float x, float y)
	{
		Vector2 bpos = board_pos();
		Vector2 mpos = { x, y };
		if (CheckCollisionPointRec(mpos, { bpos.x, bpos.y, context->board.width * fsize(), context->board.height * fsize() }))
		{
			float i = floorf((mpos.x - board_pos().x) / fsize());
			float j = floorf((mpos.y - board_pos().y) / fsize());
			DrawRectangleLinesEx(Rectangle{ board_pos().x + i * fsize(), bpos.y + j * fsize(), fsize() - 1, fsize() - 1 }, 1, BLACK);
			if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
			{
				context->player_click_on_board.first = i;
				context->player_click_on_board.second = j;
			}
		}
	}

private:
	float r = 0.4f * fsize();
};


class WinsScene : public Scene
{
public:
	WinsScene(Context* context) : Scene(context) {}

	void draw_frame() override
	{

		draw_header(context->screen_width / 2.f, context->screen_height / 3.f);
		draw_exit_btn(context->screen_width / 2.f - 5.5f * fsize(), context->screen_height / 2.f);
		draw_home_btn(context->screen_width / 2.f + 2.5f * fsize(), context->screen_height / 2.f);
		draw_next_btn(context->screen_width / 2.f - 1.5f * fsize(), context->screen_height / 2.f);
	}

	void draw_header(float x, float y)
	{
		char text[Context::plname_size + 10]{};
		if (context->bot_wins()) strcpy(text, "Bot wins!!!");
		if (context->pl1_wins()) strcat(strcpy(text, context->pl1name), " wins!!!");
		if (context->pl2_wins()) strcat(strcpy(text, context->pl2name), " wins!!!");
		draw_mdalign_text(text, x, y, 2 * fsize(), DARKGRAY);
	}

	void draw_exit_btn(float x, float y)
	{
		Rectangle rect{ x, y, 3 * fsize(), fsize()};
		if (GuiButton(rect, "Exit"))
		{
			context->scene = SceneID::exit;
		}
	}

	void draw_home_btn(float x, float y)
	{
		Rectangle rect{ x, y, 3 * fsize(), fsize() };
		if (GuiButton(rect, "Home"))
		{
			context->scene = SceneID::home;
			context->time = NULL;
		}
	}

	void draw_next_btn(float x, float y)
	{
		Rectangle rect{x, y , 3 * fsize(), fsize() };
		if (GuiButton(rect, "Next"))
		{
			context->new_board();
			if (context->vs_computer)
			{
				if (context->bot_side == Board::X) context->bot_side = Board::O;
				else if (context->bot_side == Board::O) context->bot_side = Board::X;
			}
			else std::swap(context->pl1name, context->pl2name);
			context->pass_move_to_x();
			context->scene = SceneID::play;
			context->winner = Board::_;
			context->time = NULL;
		}
	}

};


class ExitScene : public Scene
{
public:
	ExitScene(Context* context) : Scene(context) {}

	void draw() override
	{
		CloseWindow();
	}

	void draw_frame() override {};
};