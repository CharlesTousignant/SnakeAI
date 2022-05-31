#include "AIPlayer.hpp"
#include <math.h>
#include <random>

const int STATES[3] = { 1, 2, 3 };
const Direction DECISIONS[4] = { Direction::Right, Direction::Left, Direction::Up, Direction::Down };

std::random_device rd;
std::mt19937 rng(rd());

float ReLU(float x) {
    if (x > 0) {
        return x;
    }
    else {
        return 0;
    }
}

Direction AIPlayer::GetDecision(int(&boardState)[BOARD_PIXEL_COUNT])
{
    float layer1Activation[numNeuronsLayer1];
    float layer2Activation[numNeuronsLayer2];
    float outputLayerActivation[numOutputNeurons];
    float currSum;
    for (int i = 0; i < numNeuronsLayer1; i ++) {
        currSum = 0;
        for (int j = 0; j < BOARD_PIXEL_COUNT * 3; j++) {
            currSum += (boardState[j / 3] == STATES[j % 3] ? 1.0 : 0) * layer1Weight[j];
        }
        currSum += layer1Bias[i];
        layer1Activation[i] = ReLU(currSum);
    }

    for (int i = 0; i < numNeuronsLayer2; i++) {
        currSum = 0;
        for (int j = 0; j < numNeuronsLayer1; j++) {
            currSum += layer1Activation[j] * layer2Weight[j];
        }
        currSum += layer2Bias[i];
        layer2Activation[i] = ReLU(currSum);
    }

    float softMaxSum = 0;
    for (int i = 0; i < numOutputNeurons; i++) {
        currSum = 0;
        for (int j = 0; j < numNeuronsLayer2; j++) {
            currSum += layer2Activation[j] * outputLayerWeight[j];
        }
        currSum += outputLayerBias[i];
        outputLayerActivation[i] = ReLU(currSum);
        softMaxSum += exp(outputLayerActivation[i]);
    }

    int highestOutputIndex = 0;
    float highestActivationFound = 0;
    float currentActivation = 0;
    for (int i = 0; i < numOutputNeurons; i++) {
        currentActivation = (outputLayerActivation[i] == 0) ? 0 : exp(outputLayerActivation[i]) / softMaxSum;
        outputLayerActivation[i] = currentActivation;
        if (currentActivation > highestActivationFound) {
            highestActivationFound = currentActivation;
            highestOutputIndex = i;
        }
    }

    return DECISIONS[highestOutputIndex];
}

AIPlayer::AIPlayer()
{
    std::uniform_real_distribution<> dist(-1, 1);
    for (auto& weight : layer1Weight) {
        weight = dist(rng);
    }
    for (auto& weight : layer2Weight) {
        weight = dist(rng);
    }
    for (auto& weight : outputLayerWeight) {
        weight = dist(rng);
    }

    for (auto& weight : layer1Bias) {
        weight = dist(rng);
    }
    for (auto& weight : layer2Bias) {
        weight = dist(rng);
    }
    for (auto& weight : outputLayerBias) {
        weight = dist(rng);
    }
}
