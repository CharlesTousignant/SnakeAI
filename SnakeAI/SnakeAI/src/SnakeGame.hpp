#pragma once

#include <list>
#include "BoardRender.hpp"
#include <cstdlib>
#include <iostream>
#include <random>

struct Vec2D {
    char x;
    char y;
};

struct Position {
    int xPos;
    int yPos;

    Position operator+(const Position& position) const {
        return { xPos + position.xPos, yPos + position.yPos };
    }

    bool operator==(const Position& position) const {
        return xPos == position.xPos && yPos == position.yPos;
    }

    Position operator+(const Vec2D vec) {
        return { xPos + vec.x, yPos + vec.y };
    }
};

enum Direction {Right, Up, Left, Down};
enum DirectionTurn {RightTurn = -1, LeftTurn = 1, Forward = 0};
const Direction DirectionArray[4] = {
    Direction::Right,
    Direction::Up,
    Direction::Left,
    Direction::Down
};
const Position DirectionVectors[4] = {
    {1, 0},
    {0, 1},
    {-1, 0},
    {0, -1}
};

enum PixelType {Fruit, SnakeBody, SnakeHead};

class Pixel {

public:
    Position position;
    PixelType type;
    Pixel();

    Pixel(Position pos, PixelType initType);

    void SetPos(Position pos);

    void SetType(PixelType newType);
};

class Snake {

public:
    std::list<Pixel> body;
    unsigned int length;

    Snake();

    void CollectedFruit();

    Position MoveForward(Direction direction);
};

class Board {

public:

    Pixel currentFruit;
    Snake snake;
    Direction currDir;
    bool isDead;
    unsigned turnsSinceLastFruit;
    unsigned FruitsEaten;
    
    Board();

    void SetDirection(Direction newDirection);

    void MoveOneFrameForward();

    void SetStates(int(&boardState)[BOARD_PIXEL_COUNT]);

    static bool IsOnBorder(const Position& pos);
private:
 
    std::mt19937 rngGame;
    bool SnakeAteFruit();

    void SetNewFruit();

    int GetIndex(const Position& pos);

    Position GetPosition(int index);
};

