#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <ctime>
#include <chrono>

using namespace std;
using namespace std::chrono;

#define FPS 60
#define WIN_WIDTH 1200
#define WIN_HEIGHT 900
#define PLAYER_SPEED 10.0f
#define OBSTACLE_SPEED 15.0f
#define INIT_LIVES 3
#define INVINCIBILITY_DURATION 2.0f

sf::Color Pink(255,105,180);

class Base
{
protected:
    float x, y;
    float width, height;

public:
    Base(float x, float y, float width, float height) : x(x), y(y), width(width), height(height) {}
};

class Player : public Base
{
private:
    float sprPosX, sprPosY;
    float speed;

public:
    sf::Sprite knight;
    sf::Texture idle;

    Player(float x, float y, float width, float height, float sprPosX, float sprPosY)
        : Base(x, y, width, height), sprPosX(sprPosX), sprPosY(sprPosY)
    {
        idle.loadFromFile("resources/Idle.png");
        knight.setTexture(idle);
        knight.setTextureRect(sf::IntRect(sprPosX, sprPosY, width, height));
        knight.setScale(sf::Vector2f(-2, 2));
        knight.setPosition(x + width * 2, y);
        speed = PLAYER_SPEED;
    }

    void setPos(float x_, float y_)
    {
        x = x_;
        y = y_;
        knight.setPosition(x, y);
    }

    void setSpeed(float newSpeed) { speed = newSpeed; }

    void move(float offsetX, float offsetY)
    {
        x += offsetX;
        y += offsetY;
        knight.setPosition(x, y);
    }

    float getPosX() const { return x; }
    float getPosY() const { return y; }
    float getWidth() const { return width; }
    float getHeight() const { return height; }
    float getSprPosX() const { return sprPosX; }
    float getSprPosY() const { return sprPosY; }
    float getSpeed() const { return speed; }
};

class Obstacle : public Base
{
private:
    float speed;
    bool passedPlayer;

public:
    sf::RectangleShape rect;

    Obstacle(float x, float y, float width, float height, float speed)
        : Base(x, y, width, height), speed(speed), passedPlayer(false)
    {
        rect.setPosition(x, y);
        rect.setSize(sf::Vector2f(width, height));
        rect.setFillColor(sf::Color::Red);
    }

    void move(float offsetX, float offsetY)
    {
        x += offsetX;
        y += offsetY;
        rect.setPosition(x, y);
    }

    bool hasPassedPlayer() const { return passedPlayer; }
    void setPassedPlayer(bool value) { passedPlayer = value; }

    float getPosX() const { return x; }
    float getPosY() const { return y; }
    float getWidth() const { return width; }
    float getHeight() const { return height; }
    float getSpeed() const { return speed; }
};

class Elixir : public Base
{
public:
    sf::CircleShape heart;

    Elixir(float x, float y, float width, float height)
        : Base(x, y, width, height)
    {
        heart.setRadius(width / 2);
        heart.setFillColor(Pink);
        heart.setPosition(x, y);
    }
};

class Game
{
public:
    void run()
    {
        sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode();
        unsigned int screenWidth = desktopMode.width;
        unsigned int screenHeight = desktopMode.height;

        unsigned int windowWidth = WIN_WIDTH;
        unsigned int windowHeight = WIN_HEIGHT;

        unsigned int windowPosX = (screenWidth - windowWidth) / 2;
        unsigned int windowPosY = (screenHeight - windowHeight) / 2 - 50;

        sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "Knight Game", sf::Style::Default);
        window.setPosition(sf::Vector2i(windowPosX, windowPosY));
        window.setFramerateLimit(FPS);

        Player plr(500, 500, 27, 47, 49, 17);
        bool gameOver = false;
        int lives = INIT_LIVES;
        bool invincible = false;
        high_resolution_clock::time_point invincibilityTimer;
        int score = 0; // Score counter

        sf::Texture backgroundTexture;
        if (!backgroundTexture.loadFromFile("resources/background.png"))
        {
            cout << "Failed to load background image" << endl;
        }
        sf::Sprite background(backgroundTexture);
        background.setScale(sf::Vector2f(static_cast<float>(windowWidth) / background.getTexture()->getSize().x, static_cast<float>(windowHeight) / background.getTexture()->getSize().y));

        vector<Obstacle> obstacles;
        vector<Elixir> elixirs;

        srand(static_cast<unsigned int>(time(nullptr)));

        while (window.isOpen())
        {
            sf::Event event;
            while (window.pollEvent(event))
            {
                if (event.type == sf::Event::Closed)
                    window.close();

                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space && gameOver)
                {
                    plr.setPos(500, 500);
                    obstacles.clear();
                    elixirs.clear();
                    gameOver = false;
                    lives = INIT_LIVES;
                    invincible = false;
                    score = 0; // Reset score counter
                }
            }

            if (!gameOver)
            {
                handleInput(plr);
                update(plr, window);
                if (!invincible)
                {
                    spawnObstacles(obstacles);
                    updateObstacles(obstacles);
                    checkCollision(plr, obstacles, gameOver, lives, invincible, invincibilityTimer, score);

                    spawnElixirs(elixirs);
                    updateElixirs(elixirs);
                    checkElixirCollection(plr, elixirs, lives);
                }
                else
                {
                    if (isInvincibilityOver(invincibilityTimer))
                    {
                        invincible = false;
                    }
                }
            }

            render(window, plr, obstacles, elixirs, gameOver, background, lives, invincible, 0.8f, score); // Pass score to render function

            window.display();
        }
    }

private:
    void handleInput(Player &plr)
    {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
        {
            plr.move(0, -plr.getSpeed());
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
        {
            plr.move(0, plr.getSpeed());
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
        {
            plr.move(-plr.getSpeed(), 0);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
        {
            plr.move(plr.getSpeed(), 0);
        }
    }

    void update(Player &plr, sf::RenderWindow &window)
    {
        float playerLeft = plr.getPosX();
        float playerTop = plr.getPosY();
        float playerRight = playerLeft + plr.getWidth();
        float playerBottom = playerTop + plr.getHeight();

        float windowLeft = 0;
        float windowTop = 0;
        float windowRight = window.getSize().x;
        float windowBottom = window.getSize().y;

        if (playerLeft < windowLeft)
        {
            plr.setPos(windowLeft, playerTop);
        }
        if (playerTop < windowTop)
        {
            plr.setPos(playerLeft, windowTop);
        }
        if (playerRight > windowRight)
        {
            plr.setPos(windowRight - plr.getWidth(), playerTop);
        }
        if (playerBottom > windowBottom)
        {
            plr.setPos(playerLeft, windowBottom - plr.getHeight());
        }
    }

    void spawnObstacles(vector<Obstacle> &obstacles)
    {
        if (rand() % 8 == 0)
        {
            float x = -50;
            float y = static_cast<float>(rand() % WIN_HEIGHT);
            float width = 40;
            float height = 4;
            float speed = OBSTACLE_SPEED;

            Obstacle obstacle(x, y, width, height, speed);
            obstacles.push_back(obstacle);
        }
    }

    void updateObstacles(vector<Obstacle> &obstacles)
    {
        for (size_t i = 0; i < obstacles.size(); i++)
        {
            obstacles[i].move(obstacles[i].getSpeed(), 0);

            if (obstacles[i].getPosX() > WIN_WIDTH)
            {
                obstacles.erase(obstacles.begin() + i);
                i--;
            }
        }
    }

    void checkCollision(Player &plr, vector<Obstacle> &obstacles, bool &gameOver, int &lives, bool &invincible, high_resolution_clock::time_point &invincibilityTimer, int &score)
    {
        if (!invincible)
        {
            for (Obstacle &obstacle : obstacles)
            {
                if (plr.knight.getGlobalBounds().intersects(obstacle.rect.getGlobalBounds()))
                {
                    lives--;
                    if (lives == 0)
                    {
                        gameOver = true;
                    }
                    else
                    {
                        invincible = true;
                        invincibilityTimer = high_resolution_clock::now();
                    }
                    return; // Exit the function to prevent further collision checks in the same frame
                }
            }
        }
        else
        {
            if (isInvincibilityOver(invincibilityTimer))
            {
                invincible = false;
            }
        }

        for (Obstacle &obstacle : obstacles)
        {
            if (obstacle.getPosX() > plr.getPosX() + plr.getWidth() && !obstacle.hasPassedPlayer())
            {
                obstacle.setPassedPlayer(true);
                score++;
            }
        }
    }

    bool isInvincibilityOver(const high_resolution_clock::time_point &invincibilityTimer)
    {
        auto duration = duration_cast<seconds>(high_resolution_clock::now() - invincibilityTimer);
        return duration.count() >= INVINCIBILITY_DURATION;
    }

    void spawnElixirs(vector<Elixir> &elixirs)
    {
        if (rand() % 200 == 0)
        {
            float x = static_cast<float>(rand() % (WIN_WIDTH - 50));
            float y = static_cast<float>(rand() % (WIN_HEIGHT - 50));
            float width = 21;
            float height = 18;

            Elixir elixir(x, y, width, height);
            elixirs.push_back(elixir);
        }
    }

    void updateElixirs(vector<Elixir> &elixirs)
    {
        for (auto &elixir : elixirs)
        {
            // Add elixir behavior here if needed
        }
    }

    void checkElixirCollection(Player &plr, vector<Elixir> &elixirs, int &lives)
    {
        for (size_t i = 0; i < elixirs.size(); i++)
        {
            if (plr.knight.getGlobalBounds().intersects(elixirs[i].heart.getGlobalBounds()))
            {
                lives++;
                elixirs.erase(elixirs.begin() + i);
                i--;
            }
        }
    }

    void render(sf::RenderWindow &window, const Player &plr, const vector<Obstacle> &obstacles, const vector<Elixir> &elixirs, bool gameOver, const sf::Sprite &background, int lives, bool invincible, float dimness, int score)
    {
        window.clear();

        sf::RectangleShape dimmedBackground(sf::Vector2f(static_cast<float>(window.getSize().x), static_cast<float>(window.getSize().y)));
        dimmedBackground.setFillColor(sf::Color(0, 0, 0, static_cast<sf::Uint8>(255 * dimness)));
        window.draw(background);
        window.draw(dimmedBackground);

        window.draw(plr.knight);

        for (const Obstacle &obstacle : obstacles)
        {
            window.draw(obstacle.rect);
        }

        for (const Elixir &elixir : elixirs)
        {
            window.draw(elixir.heart);
        }

        sf::Font font;
        if (!font.loadFromFile("resources/font.ttf"))
        {
            cout << "Failed to load font" << endl;
            return;
        }

        sf::Text livesText;
        livesText.setFont(font);
        livesText.setString("Lives: " + to_string(lives));
        livesText.setCharacterSize(24);
        livesText.setFillColor(sf::Color::White);
        livesText.setPosition(window.getSize().x - livesText.getGlobalBounds().width - 20, 20);
        window.draw(livesText);

        sf::Text scoreText;
        scoreText.setFont(font);
        scoreText.setString("Score: " + to_string(score));
        scoreText.setCharacterSize(24);
        scoreText.setFillColor(sf::Color::White);
        scoreText.setPosition(20, 20);
        window.draw(scoreText);

        if (gameOver)
        {
            sf::Text gameOverText;
            gameOverText.setFont(font);
            gameOverText.setString("Game Over");
            gameOverText.setCharacterSize(50);
            gameOverText.setFillColor(sf::Color::Red);
            gameOverText.setPosition(WIN_WIDTH / 2 - gameOverText.getGlobalBounds().width / 2, WIN_HEIGHT / 2 - gameOverText.getGlobalBounds().height / 2);
            window.draw(gameOverText);

            sf::Text finalScoreText;
            finalScoreText.setFont(font);
            finalScoreText.setString("Final Score: " + to_string(score));
            finalScoreText.setCharacterSize(30);
            finalScoreText.setFillColor(sf::Color::White);
            finalScoreText.setPosition(WIN_WIDTH / 2 - finalScoreText.getGlobalBounds().width / 2, gameOverText.getPosition().y + gameOverText.getGlobalBounds().height + 50);
            window.draw(finalScoreText);

            sf::Text restartText;
            restartText.setFont(font);
            restartText.setString("Press SPACE to restart the game");
            restartText.setCharacterSize(24);
            restartText.setFillColor(sf::Color::White);
            restartText.setPosition(WIN_WIDTH / 2 - restartText.getGlobalBounds().width / 2, WIN_HEIGHT - restartText.getGlobalBounds().height - 50);
            window.draw(restartText);
        }

        window.display();
    }
};

int main()
{
    Game game;
    game.run();
    return 0;
}
