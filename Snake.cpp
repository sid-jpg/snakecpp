#include <SFML/Graphics.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <stdexcept>

const int blockSize = 10;
const int width = 240;
const int height = 320;

enum class Direction { Up, Down, Left, Right };

struct SnakeSegment {
    int x, y;
    SnakeSegment(int x_, int y_) : x(x_), y(y_) {}
};

class Snake {
public:
    Snake() : direction(Direction::Right) {
        reset();
    }

    void reset() {
        snake.clear();
        snake.push_back(SnakeSegment(width / (2 * blockSize), height / (2 * blockSize)));
        direction = Direction::Right;
    }

    void setDirection(Direction newDirection) {
        if ((direction == Direction::Up && newDirection != Direction::Down) ||
            (direction == Direction::Down && newDirection != Direction::Up) ||
            (direction == Direction::Left && newDirection != Direction::Right) ||
            (direction == Direction::Right && newDirection != Direction::Left)) {
            direction = newDirection;
        }
    }

    void move() {
        // Add a new head in the direction of movement
        SnakeSegment newHead = snake.front();
        switch (direction) {
        case Direction::Up:    --newHead.y; break;
        case Direction::Down:  ++newHead.y; break;
        case Direction::Left:  --newHead.x; break;
        case Direction::Right: ++newHead.x; break;
        }
        snake.insert(snake.begin(), newHead);

        // Remove the tail (unless we've just eaten)
        if (!justAte) {
            snake.pop_back();
        }
        else {
            justAte = false;
        }
    }

    void grow() {
        justAte = true;
    }

    bool checkCollisionWithSelf() const {
        if (snake.size() <= 4) return false; // Can't collide if too short

        const SnakeSegment& head = snake.front();
        for (size_t i = 4; i < snake.size(); ++i) {
            if (head.x == snake[i].x && head.y == snake[i].y) {
                return true;
            }
        }
        return false;
    }

    bool checkCollisionWithWall() const {
        const SnakeSegment& head = snake.front();
        return head.x < 0 || head.x >= width / blockSize ||
            head.y < 0 || head.y >= height / blockSize;
    }

    bool checkCollisionWithFood(int foodX, int foodY) const {
        const SnakeSegment& head = snake.front();
        return head.x == foodX && head.y == foodY;
    }

    void draw(sf::RenderWindow& window) const {
        for (const auto& segment : snake) {
            sf::RectangleShape block(sf::Vector2f(static_cast<float>(blockSize), static_cast<float>(blockSize)));
            block.setFillColor(sf::Color::Green);
            block.setPosition(static_cast<float>(segment.x * blockSize), static_cast<float>(segment.y * blockSize));
            window.draw(block);
        }
    }

private:
    std::vector<SnakeSegment> snake;
    Direction direction;
    bool justAte = false;
};

class Food {
public:
    Food() {
        placeFood();
    }

    void placeFood() {
        foodX = rand() % (width / blockSize);
        foodY = rand() % (height / blockSize);
    }

    void draw(sf::RenderWindow& window) const {
        sf::RectangleShape foodBlock(sf::Vector2f(static_cast<float>(blockSize), static_cast<float>(blockSize)));
        foodBlock.setFillColor(sf::Color::Red);
        foodBlock.setPosition(static_cast<float>(foodX * blockSize), static_cast<float>(foodY * blockSize));
        window.draw(foodBlock);
    }

    int getX() const { return foodX; }
    int getY() const { return foodY; }

private:
    int foodX, foodY;
};

int main() {
    try {
        std::cout << "Initializing game..." << std::endl;
        srand(static_cast<unsigned>(time(0)));

        sf::RenderWindow window(sf::VideoMode(width, height), "Nokia Snake Game");
        std::cout << "Window created successfully." << std::endl;

        Snake snake;
        Food food;
        int score = 0;
        bool gameOver = false;
        bool gamePaused = false;

        sf::Font font;
        std::cout << "Attempting to load font..." << std::endl;
        if (!font.loadFromFile("C:\\Windows\\Fonts\\arial.ttf")) {
            throw std::runtime_error("Failed to load font");
        }
        std::cout << "Font loaded successfully." << std::endl;

        sf::Text scoreText;
        scoreText.setFont(font);
        scoreText.setCharacterSize(15);
        scoreText.setFillColor(sf::Color::White);
        scoreText.setPosition(5, 5);

        sf::Text gameOverText;
        gameOverText.setFont(font);
        gameOverText.setCharacterSize(20);
        gameOverText.setFillColor(sf::Color::White);
        gameOverText.setPosition(width / 4, height / 3);

        sf::Clock clock;
        std::cout << "Entering main game loop..." << std::endl;
        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed)
                    window.close();
                if (event.type == sf::Event::KeyPressed) {
                    if (event.key.code == sf::Keyboard::Up)
                        snake.setDirection(Direction::Up);
                    else if (event.key.code == sf::Keyboard::Down)
                        snake.setDirection(Direction::Down);
                    else if (event.key.code == sf::Keyboard::Left)
                        snake.setDirection(Direction::Left);
                    else if (event.key.code == sf::Keyboard::Right)
                        snake.setDirection(Direction::Right);
                    else if (event.key.code == sf::Keyboard::Space) {
                        if (gameOver) {
                            gameOver = false;
                            snake.reset();
                            food.placeFood();
                            score = 0;
                        }
                        else {
                            gamePaused = !gamePaused;
                        }
                    }
                }
            }

            if (!gameOver && !gamePaused && clock.getElapsedTime().asMilliseconds() > 100) {
                snake.move();

                if (snake.checkCollisionWithFood(food.getX(), food.getY())) {
                    snake.grow();
                    food.placeFood();
                    ++score;
                }

                gameOver = snake.checkCollisionWithSelf() || snake.checkCollisionWithWall();

                clock.restart();
            }

            window.clear(sf::Color::Black);

            snake.draw(window);
            food.draw(window);

            scoreText.setString("Score: " + std::to_string(score));
            window.draw(scoreText);

            if (gameOver) {
                gameOverText.setString("Game Over\nScore: " + std::to_string(score) + "\nPress Space to Restart");
                window.draw(gameOverText);
            }
            else if (gamePaused) {
                gameOverText.setString("Paused\nPress Space to Resume");
                window.draw(gameOverText);
            }

            window.display();
        }
    }
    catch (const std::exception& e) {
        std::cerr << "An error occurred: " << e.what() << std::endl;
        std::cout << "Press Enter to exit...";
        std::cin.get();
        return 1;
    }
    catch (...) {
        std::cerr << "An unknown error occurred." << std::endl;
        std::cout << "Press Enter to exit...";
        std::cin.get();
        return 1;
    }

    return 0;
}