#pragma once

#include <list>
#include "BoardRender.h"
#include <cstdlib>
#include <iostream>
#include <random>

//static  std::mt19937 rngGame;
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
    Pixel() {
        position = { 0, 0 };
        type = PixelType::Fruit;
    }

    Pixel(Position pos, PixelType initType) {
        position = pos;
        type = initType;
    }

    void SetPos(Position pos) {
        position = pos;
    }

    void SetType(PixelType newType) {
        type = newType;
    }
};

class Snake {

public:
    std::list<Pixel> body;
    unsigned int length;

    Snake() {
        length = 3;
        body.push_back(Pixel( { 8, 8 }, PixelType::SnakeBody) );
        body.push_back(Pixel({ 7, 8 }, PixelType::SnakeBody));
        body.push_back(Pixel({ 6, 8 }, PixelType::SnakeBody));
        body.push_back(Pixel({ 5, 8 }, PixelType::SnakeBody));
        body.push_back(Pixel({ 4, 8 }, PixelType::SnakeBody));
    }

    void CollectedFruit() {
        body.push_back(Pixel(body.back().position, PixelType::SnakeBody));
    }

    Position MoveForward(Direction direction) {

        auto headPosition = body.front().position;
        auto lastBody = body.back();
        Position lastBodyPos = lastBody.position;
        lastBody.SetPos(headPosition + DirectionVectors[direction]);
        body.push_front(lastBody);
        body.pop_back();
        return lastBodyPos;
    }
};

class Board {

public:

    Pixel currentFruit;
    Snake snake;
    Direction currDir;
    bool isDead;
    unsigned turnsDone = 0;
    unsigned turnsSinceLastFruit = 0;
    unsigned FruitsEaten = 0;
    
    Board() {
        snake = Snake();
        SetNewFruit();
        currDir = Direction::Right;
        isDead = false;
        
    }

    void SetDirection(Direction newDirection) {
        if (currDir == Direction::Right && newDirection == Direction::Left ||
            currDir == Direction::Left && newDirection == Direction::Right ||
            currDir == Direction::Up && newDirection == Direction::Down ||
            currDir == Direction::Down && newDirection == Direction::Up) return; // Can only turn left or right
        currDir = newDirection;
    }

    void SetDirectionTurn(DirectionTurn direction) {
        if (currDir == 0) {
            if (direction == DirectionTurn::RightTurn) {
                currDir = Direction::Down;
            }
            else if (direction == DirectionTurn::LeftTurn) {
                currDir = Direction::Up;
            }
        }
        else {
            currDir = DirectionArray[(currDir + direction) % 4];
        }
    }

    void MoveOneFrameForward() {
        if (isDead) {
            std::cout << "You lose :(" << std::endl;
            return;
        }
        auto endPos = snake.MoveForward(currDir);
        for (auto it = ++snake.body.begin(); it != snake.body.end(); it++) {
            if (it->position == snake.body.front().position) {
                isDead = true;
            }
        }
        isDead |= IsOnBorder(snake.body.front().position);
        if (SnakeAteFruit()) {
            FruitsEaten++;
            snake.body.push_back(Pixel(endPos, PixelType::SnakeBody));
            SetNewFruit();
        }
        turnsSinceLastFruit++;
        turnsDone++;
    }

    void SetStates(int (&boardState) [BOARD_PIXEL_COUNT]) {
        if (isDead) {
            //std::cout << "You lose :(" << std::endl;
            return;
        }
        for (auto& state : boardState) {
            state = 0;
        }

        boardState[GetIndex(currentFruit.position)] = 1;
        boardState[GetIndex(snake.body.front().position)] = 2;
        for (auto it = ++snake.body.begin(); it != snake.body.end(); it++) {
            boardState[GetIndex(it->position)] = 3;
        }
    }

    static bool IsOnBorder(const Position& pos) {
        return pos.xPos == -1 || pos.xPos == BOARD_DIMENSIONS || pos.yPos == -1 || pos.yPos == BOARD_DIMENSIONS;
    }
private:
 
    std::mt19937 rngGame;
    bool SnakeAteFruit() {
        auto snakeHeadPos = snake.body.front().position;
        return snakeHeadPos == currentFruit.position;
    }

    void SetNewFruit() {
        turnsSinceLastFruit = 0;
        Position newPos;
        bool posInvalid = true;
        while (posInvalid) {
            newPos = GetPosition(rngGame() / float(0xFFFFFFFF) * BOARD_PIXEL_COUNT);
            posInvalid = false;
            for (const auto& body : snake.body) {
                posInvalid |= body.position == newPos;
            }
        }
        //std::cout << "Set fruit pos: " << newPos.xPos << "," << newPos.yPos << std::endl;
        currentFruit = Pixel(newPos, PixelType::Fruit);
    }

    int GetIndex(const Position& pos) {
        return pos.xPos + pos.yPos * BOARD_DIMENSIONS;
    }

    Position GetPosition(int index) {
        return { index % BOARD_DIMENSIONS, index / BOARD_DIMENSIONS };
    }

};

