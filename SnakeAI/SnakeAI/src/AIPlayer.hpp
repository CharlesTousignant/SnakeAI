#pragma once
#include "SnakeGame.h"
#include <array>
#include <random>
#include <memory>

#define numNeuronsLayer1 16
#define numNeuronsLayer2 8
#define numOutputNeurons 4

struct SnakeViewState {
    char directions[8][3];
};

class AIPlayer {
public:
    Direction GetDecision(int(&boardState)[BOARD_PIXEL_COUNT], Board& board);
    AIPlayer();

    void ConvertToChild(const AIPlayer& player1, const AIPlayer& player2);

private:
    std::array<float, 8 * 3 * numNeuronsLayer1> layer1Weight;
    std::array<float, numNeuronsLayer1* numNeuronsLayer2> layer2Weight;
    std::array<float, numNeuronsLayer2 * numOutputNeurons> outputLayerWeight;
    std::array<float, numNeuronsLayer1> layer1Bias;
    std::array<float, numNeuronsLayer2> layer2Bias;
    std::array<float, numOutputNeurons> outputLayerBias;

    template<int N>
    void RandomiseValues(std::array<float, N>& toFill, const std::array<float, N>& parent1Values, const std::array<float, N>& parent2Values);

    SnakeViewState GetView(int(&boardState)[BOARD_PIXEL_COUNT], Board& board);
    std::uniform_real_distribution<> distInit;
    std::normal_distribution<> distMutation;
};