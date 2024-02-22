#include <SFML/Graphics.hpp>
#include <random>

//using namespace std;
//using namespace sf;

int main()
{
    // My window dimensions - deletable - changable
    const int windowWidth = 800;
    const int windowHeight = 600;

    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "Random Platforms");

    //randomness generator
    std::random_device rd;
    std::mt19937 rng(rd());     //random.h: typdef for twister engine
    std::uniform_int_distribution<int> xDist(0, windowWidth - 50);
    std::uniform_int_distribution<int> yDist(0, windowHeight - 20);

    //creating platform vector
    std::vector<sf::RectangleShape> platformsv;

    //platform generator
    for (int i = 0; i < 10; ++i)
    {
        int platformWidth = 50;
        int platformHeight = 20;
        int platformX = xDist(rng);
        int platformY = yDist(rng);

        sf::RectangleShape platform(sf::Vector2f(platformWidth, platformHeight));
        platform.setPosition(platformX, platformY);
        platform.setFillColor(sf::Color::Blue);

        platformsv.push_back(platform);
    }

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear(sf::Color::Black);

        for (const auto& platform : platformsv)
            window.draw(platform);

        window.display();
    }

    return 0;
}