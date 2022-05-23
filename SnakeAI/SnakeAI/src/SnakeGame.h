#pragma once

#include <list>
#include "BoardRender.h"
#include <cstdlib>
#include <iostream>

struct Position {
    unsigned int xPos;
    unsigned int yPos;

    Position operator+(const Position& position) const {
        return { xPos + position.xPos, yPos + position.yPos };
    }

    bool operator==(const Position& position) const {
        return xPos == position.xPos && yPos == position.yPos;
    }
};



enum Direction {Right, Left, Up, Down};
const Position DirectionVectors[4] = {
    {1, 0},
    {-1, 0},
    {0, 1},
    {0, -1}
};

enum PixelType {Fruit, SnakeBody};

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
        body.push_back(Pixel( { 32, 30 }, PixelType::SnakeBody) );
        body.push_back(Pixel({ 31, 30 }, PixelType::SnakeBody));
        body.push_back(Pixel({ 30, 30 }, PixelType::SnakeBody));
        body.push_back(Pixel({ 29, 30 }, PixelType::SnakeBody));
        body.push_back(Pixel({ 28, 30 }, PixelType::SnakeBody));
        body.push_back(Pixel({ 27, 30 }, PixelType::SnakeBody));
        //body.push_back(Pixel({ 29, 32 }, PixelType::SnakeBody));
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
    unsigned FruitsEaten = 0;
    Board() {
        snake = Snake();
        currentFruit = Pixel({ 40, 30 }, PixelType::Fruit);
        currDir = Direction::Right;
        isDead = false;
    }

    void SetDirection(Direction newDirection) {
        currDir = newDirection;
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
        turnsDone++;
    }

    void SetStates(int (&boardState) [BOARD_PIXEL_COUNT]) {

        for (auto& state : boardState) {
            state = 0;
        }

        boardState[GetIndex(currentFruit.position)] = 1;
        for (const auto& body : snake.body) {
            boardState[GetIndex(body.position)] = 1;
        }
    }

private:
    bool SnakeAteFruit() {
        auto snakeHeadPos = snake.body.front().position;
        return snakeHeadPos == currentFruit.position;
    }

    void SetNewFruit() {
        Position newPos;
        bool posInvalid = true;
        while (posInvalid) {
            newPos = GetPosition(rand() / float(RAND_MAX) * BOARD_DIMENSIONS);
            posInvalid = false;
            for (const auto& body : snake.body) {
                posInvalid |= body.position == newPos;
            }
        }
        std::cout << "Set fruit pos: " << newPos.xPos << "," << newPos.yPos << std::endl;
        currentFruit = Pixel(newPos, PixelType::Fruit);
    }

    int GetIndex(const Position& pos) {
        return pos.xPos + pos.yPos * 64;
    }

    Position GetPosition(unsigned index) {
        return { index % BOARD_DIMENSIONS, index / BOARD_DIMENSIONS };
    }

    bool IsOnBorder(const Position& pos) {
        return pos.xPos == 0 || pos.xPos == BOARD_DIMENSIONS - 1 || pos.yPos == 0 || pos.yPos == BOARD_DIMENSIONS - 1;
    }
};

