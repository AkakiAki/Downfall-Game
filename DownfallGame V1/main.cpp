#include <SFML/Graphics.hpp>
#include <string>

using namespace std;

#define FPS					60
#define WIN_MULTIPLIER		1.f
#define WIN_WIDTH			1200.f * WIN_MULTIPLIER
#define WIN_HEIGHT			900.f * WIN_MULTIPLIER
#define G_DEFAULT			8.f
#define PLAYER_WIDTH		20.f
#define PLAYER_HEIGHT		50.f // 50px = 1 meter
#define MOVE_SPEED			5.f
#define CENTER				WIN_WIDTH / 2 - PLAYER_WIDTH / 2
#define FOOT				WIN_HEIGHT - PLAYER_HEIGHT

class Game
{
public:
	Game()
		: window(sf::VideoMode(WIN_WIDTH, WIN_HEIGHT), "Downfall"),
		  player(sf::Vector2f(PLAYER_WIDTH, PLAYER_HEIGHT)),
		  depthCounter(),
		  px(WIN_WIDTH / 2 - PLAYER_WIDTH / 2),
		  py(0),
		  G(G_DEFAULT),
		  acc(0.1),
		  jump(0),
		  depth(0)
	{
		setupWindow();
		setupPlayer();
		setupDepthCounter();
	}

	void run()
	{
		while (window.isOpen())
		{
			handleEvents();
			update();
			render();
		}
	}

private:
	sf::RenderWindow window;
	sf::RectangleShape player;
	sf::Font font;
	sf::Text depthCounter;
	float px;
	float py;
	float G;
	float acc;
	int jump;
	int depth;

	void setupWindow()
	{
		window.setFramerateLimit(FPS);
	}

	void setupPlayer()
	{
		player.setFillColor(sf::Color::Blue);
		player.setPosition(px, py);
	}

	void setupDepthCounter()
	{
		if (!font.loadFromFile("font.ttf"))
		{
			// Handle font loading error
		}
		depthCounter.setFont(font);
		depthCounter.setCharacterSize(24);
		depthCounter.setFillColor(sf::Color::White);
		depthCounter.setPosition(5, 5);
	}

	void handleEvents()
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				window.close();
			}
		}
	}

	void update()
	{
		handleInput();
		updatePlayerPosition();
		updateDepth();
	}

	void handleInput()
	{
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
			player.move(-MOVE_SPEED, 0.f);
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
			player.move(MOVE_SPEED, 0.f);

		if (!jump)
		{
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
			{
				G = -4.f;
				player.move(0.f, G);
				py += G;
				jump += 1;
			}
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
		{
			player.setPosition(CENTER, 0);
			G = G_DEFAULT;
			py = 0;
		}

		G += acc;
	}

	void updatePlayerPosition()
	{
		if (py >= FOOT)
		{
			G = 0;
			jump = 0;
		}
		else
		{
			player.move(0.f, G);
			py += G;
		}
	}

	void updateDepth()
	{
		depthCounter.setString("depth: " + to_string((int)py / 50) + "m");
	}

	void render()
	{
		window.clear();
		window.draw(depthCounter);
		window.draw(player);
		window.display();
	}
};

int main()
{
	Game game;
	game.run();

	return 0;
}
