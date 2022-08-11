#pragma once

#include "SnakeGame.hpp"


Pixel::Pixel()
{
    position = { 0, 0 };
    type = PixelType::Fruit;
}

Pixel::Pixel(Position pos, PixelType initType)
{
    position = pos;
    type = initType;
}

void Pixel::SetPos(Position pos)
{
    position = pos;
}

void Pixel::SetType(PixelType newType)
{
    type = newType;
}

Snake::Snake()
{
    length = 3;
    body.push_back(Pixel({ 8, 8 }, PixelType::SnakeBody));
    body.push_back(Pixel({ 7, 8 }, PixelType::SnakeBody));
    body.push_back(Pixel({ 6, 8 }, PixelType::SnakeBody));
    body.push_back(Pixel({ 5, 8 }, PixelType::SnakeBody));
    body.push_back(Pixel({ 4, 8 }, PixelType::SnakeBody));
}

void Snake::CollectedFruit()
{
    body.push_back(Pixel(body.back().position, PixelType::SnakeBody));
}

Position Snake::MoveForward(Direction direction)
{

    auto headPosition = body.front().position;
    auto lastBody = body.back();
    Position lastBodyPos = lastBody.position;
    lastBody.SetPos(headPosition + DirectionVectors[direction]);
    body.push_front(lastBody);
    body.pop_back();
    return lastBodyPos;
}

Board::Board()
{
    snake = Snake();
    SetNewFruit();
    currDir = Direction::Right;
    isDead = false;
    turnsSinceLastFruit = 0;
    FruitsEaten = 0;

}


void Board::SetDirection(Direction newDirection)
{
    if (currDir == Direction::Right && newDirection == Direction::Left ||
        currDir == Direction::Left  && newDirection == Direction::Right ||
        currDir == Direction::Up    && newDirection == Direction::Down    ||
        currDir == Direction::Down  && newDirection == Direction::Up) return; // Can only turn left or right
    currDir = newDirection;
}

void Board::MoveOneFrameForward()
{
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
}



void Board::SetStates(int(&boardState)[BOARD_PIXEL_COUNT]) {
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

bool Board::IsOnBorder(const Position& pos)
{
    return pos.xPos == -1 || pos.xPos == BOARD_DIMENSIONS || pos.yPos == -1 || pos.yPos == BOARD_DIMENSIONS;
}

bool Board::SnakeAteFruit()
{
    /*std::cout << "Ate fruit! \n";*/
    auto snakeHeadPos = snake.body.front().position;
    return snakeHeadPos == currentFruit.position;
}

void Board::SetNewFruit()
{
    turnsSinceLastFruit = 0;
    Position newPos = { 0, 0 };
    bool posInvalid = true;
    while (posInvalid) {
        newPos = GetPosition(rngGame() / float(0xFFFFFFFF) * BOARD_PIXEL_COUNT);
        //newPos = { 16, 8 };
        posInvalid = false;
        for (const auto& body : snake.body) {
            posInvalid |= body.position == newPos;
        }
    }
    //std::cout << "Set fruit pos: " << newPos.xPos << "," << newPos.yPos << std::endl;
    currentFruit = Pixel(newPos, PixelType::Fruit);
}

int Board::GetIndex(const Position& pos) {
    return pos.xPos + pos.yPos * BOARD_DIMENSIONS;
}

Position Board::GetPosition(int index) {
    return { index % BOARD_DIMENSIONS, index / BOARD_DIMENSIONS };
}