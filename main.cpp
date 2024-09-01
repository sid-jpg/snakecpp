#include <SFML/Graphics.hpp>
#include <vector>
#include <deque>
#include <cstdlib>
#include <ctime>

const int WIDTH = 800;
const int HEIGHT = 600;
const int SIZE = 20;

class Snake
{
public:
    Snake()
    {
        body.push_front(sf::Vector2i(WIDTH / 2, HEIGHT / 2));
        direction = sf::Vector2i(SIZE, 0);
        grow = false;
    }

    void handleInput()
    {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && direction.x == 0)
            direction = sf::Vector2i(-SIZE, 0);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) && direction.x == 0)
            direction = sf::Vector2i(SIZE, 0);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && direction.y == 0)
            direction = sf::Vector2i(0, -SIZE);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) && direction.y == 0)
            direction = sf::Vector2i(0, SIZE);
    }

    void update()
    {
        sf::Vector2i newHead = body.front() + direction;

        if (grow)
        {
            grow = false;
        }
        else
        {
            body.pop_back();
        }

        body.push_front(newHead);

        // Check for collision with self or walls
        if (body.front().x < 0 || body.front().x >= WIDTH || body.front().y < 0 || body.front().y >= HEIGHT)
        {
            reset();
        }
        for (auto it = body.begin() + 1; it != body.end(); ++it)
        {
            if (*it == body.front())
            {
                reset();
            }
        }
    }

    void growSnake()
    {
        grow = true;
    }

    void reset()
    {
        body.clear();
        body.push_front(sf::Vector2i(WIDTH / 2, HEIGHT / 2));
        direction = sf::Vector2i(SIZE, 0);
    }

    void draw(sf::RenderWindow &window)
    {
        for (auto &segment : body)
        {
            sf::RectangleShape rectangle(sf::Vector2f(SIZE, SIZE));
            rectangle.setFillColor(sf::Color::Green);
            rectangle.setPosition(segment.x, segment.y);
            window.draw(rectangle);
        }
    }

    std::vector<sf::Vector2i> body;
    sf::Vector2i direction;
    bool grow;
};

class Food
{
public:
    Food()
    {
        srand(time(0));
        spawn();
    }

    void spawn()
    {
        position.x = (rand() % (WIDTH / SIZE)) * SIZE;
        position.y = (rand() % (HEIGHT / SIZE)) * SIZE;
    }

    void draw(sf::RenderWindow &window)
    {
        sf::RectangleShape rectangle(sf::Vector2f(SIZE, SIZE));
        rectangle.setFillColor(sf::Color::Red);
        rectangle.setPosition(position.x, position.y);
        window.draw(rectangle);
    }

    sf::Vector2i position;
};

int main()
{
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Snake Game");
    Snake snake;
    Food food;

    sf::Clock clock;
    float timer = 0.0f;
    float delay = 0.1f;

    while (window.isOpen())
    {
        sf::Time deltaTime = clock.restart();
        timer += deltaTime.asSeconds();

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        if (timer > delay)
        {
            snake.handleInput();
            snake.update();

            if (snake.body.front() == food.position)
            {
                snake.growSnake();
                food.spawn();
            }

            timer = 0;
        }

        window.clear();
        snake.draw(window);
        food.draw(window);
        window.display();
    }

    return 0;
}
