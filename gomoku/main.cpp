#ifndef _DEBUG
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#endif // !_DEBUG

#include "scene.hpp"
#include "bot.h"


class Game
{
public:
	Bot bot;
	Context* context;
	std::map<SceneID, Scene*> registered_scenes;

	~Game() { delete context; }
	Game() : context{ new Context } { bot = Bot(context); };

	void register_scene(Scene* scene, SceneID reson) { registered_scenes[reson] = scene; }

	void draw_scene() { return draw_scene(context->scene); }
	void draw_scene(SceneID reson) { return registered_scenes[reson]->draw(); }

	void gameloop()
	{
		while (!WindowShouldClose())
		{
			draw_scene();

			if (context->scene == SceneID::play)
			{
				if (context->human_move())
				{
					if (context->make_move(context->player_click_on_board))
					{
						context->winner = context->board.check_win();
						context->clear_click_on_board();
					}	
				}
				else
				{
					if (context->make_move(bot.move()))
						context->winner = context->board.check_win();
				}
			}
		}
	}

};


int main()
{
	Game game;
	game.register_scene(new InitScene(game.context), SceneID::init);
	game.register_scene(new HomeScene(game.context), SceneID::home);
	game.register_scene(new SettingsScene(game.context), SceneID::settings);
	game.register_scene(new PlayScene(game.context), SceneID::play);
	game.register_scene(new WinsScene(game.context), SceneID::wins);
	game.register_scene(new ExitScene(game.context), SceneID::exit);


	game.gameloop();
}
