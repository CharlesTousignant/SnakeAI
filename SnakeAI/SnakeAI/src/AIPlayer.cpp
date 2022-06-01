#include "AIPlayer.hpp"
#include <math.h>

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
    for (int i = 0; i < numNeuronsLayer1; i++) {
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
    distInit = std::uniform_real_distribution<>(-10, 10);
    distMutation = std::normal_distribution<>(0, 10);
    for (auto& weight : layer1Weight) {
        weight = float(rng()) / 0xFFFFFFFF - 0x8FFFFFFF;
    }
    for (auto& weight : layer2Weight) {
        weight = float(rng()) / 0xFFFFFFFF - 0x8FFFFFFF;
    }
    for (auto& weight : outputLayerWeight) {
        weight = float(rng()) / 0xFFFFFFFF - 0x8FFFFFFF;
    }

    for (auto& weight : layer1Bias) {
        weight = float(rng()) / 0xFFFFFFFF - 0x8FFFFFFF;
    }
    for (auto& weight : layer2Bias) {
        weight = float(rng()) / 0xFFFFFFFF - 0x8FFFFFFF;
    }
    for (auto& weight : outputLayerBias) {
        weight = float(rng()) / 0xFFFFFFFF - 0x8FFFFFFF;
    }
}

//AIPlayer::SetParams( std::unique_ptr<std::array<float, BOARD_PIXEL_COUNT * 3 * numNeuronsLayer1>> layer1Weight,
//                    std::unique_ptr<std::array<float, numNeuronsLayer1* numNeuronsLayer2>> layer2Weight,
//                    std::unique_ptr<std::array<float, numNeuronsLayer2* numOutputNeurons>> outputLayerWeight,
//                    std::unique_ptr<std::array<float, numNeuronsLayer1>> layer1Bias,
//                    std::unique_ptr<std::array<float, numNeuronsLayer2>> layer2Bias,
//                    std::unique_ptr<std::array<float, numOutputNeurons>> outputLayerBias)
//{   
//
//    this->layer1Weight = std::make_unique<std::array<float, BOARD_PIXEL_COUNT * 3 * numNeuronsLayer1>>(layer1Weight.release());
//    this->layer2Weight = std::make_unique<std::array<float, numNeuronsLayer1* numNeuronsLayer2>>(layer2Weight.release());
//    this->outputLayerWeight = std::make_unique<std::array<float, numNeuronsLayer2* numOutputNeurons>>(outputLayerWeight.release());
//
//    this->layer1Bias = std::make_unique<std::array<float, numNeuronsLayer1>>(layer1Bias.release());
//    this->layer2Bias = std::make_unique<std::array<float, numNeuronsLayer2>>(layer2Bias.release());
//    this->outputLayerBias = std::make_unique<std::array<float, numOutputNeurons>>(outputLayerBias.release());
//
//}

void AIPlayer::ConvertToChild(const AIPlayer& player)
{   

    RandomiseValues< BOARD_PIXEL_COUNT * 3 * numNeuronsLayer1>(this->layer1Weight, player.layer1Weight);
    RandomiseValues< numNeuronsLayer1* numNeuronsLayer2>(this->layer2Weight, player.layer2Weight);
    RandomiseValues< numNeuronsLayer2* numOutputNeurons>(this->outputLayerWeight, player.outputLayerWeight);
    RandomiseValues< numNeuronsLayer1 >(this->layer1Bias, player.layer1Bias);
    RandomiseValues< numNeuronsLayer2 >(this->layer2Bias, player.layer2Bias);
    RandomiseValues< numOutputNeurons >(this->outputLayerBias, player.outputLayerBias);
}

template<int N>
void AIPlayer::RandomiseValues(std::array<float, N>& toFill, const std::array<float, N>& toUse){
    for (int i = 0; i < N; i++) {
        if (rng() % 3 != 0) {
            toFill[i] = toUse[i] + distMutation(rng);
        }
        else {
            toFill[i] = toUse[i];
        }
    }
}
