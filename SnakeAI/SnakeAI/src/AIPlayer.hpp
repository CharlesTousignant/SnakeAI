#pragma once
#include "SnakeGame.hpp"
#include <array>
#include <random>
#include <memory>

#define numNeuronsLayer1 16
#define numNeuronsLayer2 8
#define numOutputNeurons 4

struct SnakeViewState {
    int directions[8][3];
};

class AIPlayer {
public:
    Direction GetDecision(Board& board) const;
    AIPlayer();

    void ConvertToChild(const AIPlayer& player1, const AIPlayer& player2);

private:
    std::array<double, 8 * 3 * numNeuronsLayer1> layer1Weight{};
    std::array<double, numNeuronsLayer1* numNeuronsLayer2> layer2Weight{};
    std::array<double, numNeuronsLayer2 * numOutputNeurons> outputLayerWeight{};
    std::array<double, numNeuronsLayer1> layer1Bias{};
    std::array<double, numNeuronsLayer2> layer2Bias{};
    std::array<double, numOutputNeurons> outputLayerBias{};

    template<int N>
    void RandomiseValues(std::array<double, N>& toFill, const std::array<double, N>& parent1Values, const std::array<double, N>& parent2Values);

    SnakeViewState GetView(Board& board) const;
    std::uniform_real_distribution<> distInit;
    std::normal_distribution<> distMutation;
};