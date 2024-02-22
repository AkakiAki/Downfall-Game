#include <SFML/Graphics.hpp>
#include <vector>
#include <random>

using namespace std;

#define FPS 60
#define WIN_WIDTH 1200
#define WIN_HEIGHT 900

// custom colors
sf::Color stoneGray(80, 80, 80);

class Base
{
protected:
    float width, height;
    float px, py;
    const float gravity = 1;
};

class Player : public Base
{
protected:
    sf::Sprite knight;
    float movementSpeed;
    float velocity;
    float minX, minY, maxX, maxY;
    bool isFlipped;
    bool isJumping;
    bool isDucking;

public:
    float getPositionX()
    {
        return px;
    }
    float getPositionY()
    {
        return py;
    }
    void setPosition(float X, float Y)
    {
        px = X;
        py = Y;
    }
    void flip()
    {
    }
    void jump()
    {
    }
};

class Bullet : public Base
{
private:
    sf::CircleShape shape;
    float speed;

public:
    Bullet(float x, float y)
    {
        shape.setRadius(5.f);
        shape.setFillColor(sf::Color::Red);
        shape.setPosition(x, y);
        speed = 5.f;
    }

    void update()
    {
        shape.move(0.f, speed);
    }

    void draw(sf::RenderWindow& window) const
    {
        window.draw(shape);
    }

    float getPositionY() const
    {
        return shape.getPosition().y;
    }
};

class Game
{
public:
    Player plr;
    vector<Bullet> bullets;

    void generateBullet()
    {
        random_device rd;
        mt19937 rng(rd());
        uniform_int_distribution<int> xDist(0, WIN_WIDTH - 10);

        float x = xDist(rng);
        float y = 0.f;

        Bullet bullet(x, y);
        bullets.push_back(bullet);
    }

    void run()
    {
        sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode();
        unsigned int screenWidth = desktopMode.width;
        unsigned int screenHeight = desktopMode.height;

        unsigned int windowWidth = WIN_WIDTH;
        unsigned int windowHeight = WIN_HEIGHT;

        unsigned int windowPosX = (screenWidth - windowWidth) / 2;
        unsigned int windowPosY = (screenHeight - windowHeight) / 2 - 20;

        sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "Knight Game", sf::Style::Default);
        window.setPosition(sf::Vector2i(windowPosX, windowPosY));
        window.setFramerateLimit(FPS);

        sf::Texture backgroundTexture;
        if (!backgroundTexture.loadFromFile("resources/background.jpg"))
        {
            // ...
        }

        sf::Sprite backgroundSprite;
        backgroundSprite.setTexture(backgroundTexture);

        float H = 36;
        float W = 18;
        int n = 0;
        int spr[6] = {13, 55, 98, 140, 181, 222};

        // PLAYER
        sf::Texture t1;
        t1.loadFromFile("resources/knight walk animation.png");

        sf::Sprite knight(t1);
        knight.setTextureRect(sf::IntRect(spr[n], 4, W, H));
        knight.setScale(sf::Vector2f(3, 3));
        knight.setOrigin(9, 18);
        knight.setPosition(500, WIN_HEIGHT - 89);

        bool isFlipped = false;
        const float movementSpeed = 4.f;

        sf::Clock clock;
        float frameDuration = 1.0f / 1.0f; // Frame duration for 60 FPS

        sf::Clock bulletTimer;
        const float bulletInterval = 1.0f; // Time interval between generating bullets

        while (window.isOpen())
        {
            sf::Event event;
            while (window.pollEvent(event))
            {
                if (event.type == sf::Event::Closed)
                    window.close();
            }

            // Store the previous position of the player
            sf::Vector2f prevPos = knight.getPosition();

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
            {
                knight.move(0, -movementSpeed);
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
            {
                knight.move(0, movementSpeed);
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
            {
                if (!isFlipped)
                {
                    knight.setScale(-3.f, 3.f); // Flip horizontally
                    isFlipped = true;
                }
                knight.move(-movementSpeed, 0);
                if (n >= 5)
                {
                    n = 0;
                }
                else
                {
                    n++;
                }
                knight.setTextureRect(sf::IntRect(spr[n], 4, W, H));
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
            {
                if (isFlipped)
                {
                    knight.setScale(3.f, 3.f); // Reset scaling
                    isFlipped = false;
                }
                knight.move(movementSpeed, 0);
                if (n >= 5)
                {
                    n = 0;
                }
                else
                {
                    n++;
                }
                knight.setTextureRect(sf::IntRect(spr[n], 4, W, H));
            }

            // Check if the player sprite is outside the window boundaries
            if (knight.getPosition().x < 0 || knight.getPosition().x > windowWidth ||
                knight.getPosition().y < 0 || knight.getPosition().y > windowHeight)
            {
                // If outside, restore the previous position
                knight.setPosition(prevPos);
            }

            // Update and remove bullets
            for (int i = bullets.size() - 1; i >= 0; --i)
            {
                bullets[i].update();
                if (bullets[i].getPositionY() > windowHeight)
                {
                    bullets.erase(bullets.begin() + i);
                }
            }

            // Generate new bullet after a certain time interval
            if (bulletTimer.getElapsedTime().asSeconds() > bulletInterval)
            {
                generateBullet();
                bulletTimer.restart();
            }

            window.clear();
            window.draw(backgroundSprite);

            // Draw bullets
            for (const auto& bullet : bullets)
            {
                bullet.draw(window);
            }

            window.draw(knight);
            window.display();

            // Adjust animation speed
            float elapsedTime = clock.restart().asSeconds();
            if (elapsedTime > 0.f)
            {
                float animationSpeed = 1.f / elapsedTime; // Frames per second
                frameDuration = 1.0f / animationSpeed;
            }
        }
    }
};

int main()
{
    Game game;
    game.run();

    return 0;
}