#pragma once
#include "SnakeGame.h"
#include <array>
#include <random>
#include <memory>

#define numNeuronsLayer1 75
#define numNeuronsLayer2 30
#define numOutputNeurons 4
class AIPlayer {
public:
    Direction GetDecision(int(&boardState)[BOARD_PIXEL_COUNT]);
    AIPlayer();
    /*void SetParams(std::unique_ptr < std::array<float, BOARD_PIXEL_COUNT * 3 * numNeuronsLayer1>> layer1Weight,
             std::unique_ptr<std::array<float, numNeuronsLayer1* numNeuronsLayer2>> layer2Weight,
             std::unique_ptr<std::array<float, numNeuronsLayer2* numOutputNeurons>> outputLayerWeight,
             std::unique_ptr<std::array<float, numNeuronsLayer1>> layer1Bias,
             std::unique_ptr<std::array<float, numNeuronsLayer2>> layer2Bias,
             std::unique_ptr<std::array<float, numOutputNeurons>> outputLayerBias);*/
    void ConvertToChild(const AIPlayer& player);

private:
    std::array<float, BOARD_PIXEL_COUNT * 3 * numNeuronsLayer1> layer1Weight;
    std::array<float, numNeuronsLayer1* numNeuronsLayer2> layer2Weight;
    std::array<float, numNeuronsLayer2 * numOutputNeurons> outputLayerWeight;
    std::array<float, numNeuronsLayer1> layer1Bias;
    std::array<float, numNeuronsLayer2> layer2Bias;
    std::array<float, numOutputNeurons> outputLayerBias;

    template<int N>
    void RandomiseValues(std::array<float, N>& toFill, const std::array<float, N>& toUse);

    std::uniform_real_distribution<> distInit;
    std::normal_distribution<> distMutation;
};