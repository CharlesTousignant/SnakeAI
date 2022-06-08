#include "AIPlayer.hpp"
#include <math.h>

//const int STATES[3] = { 1, 2, 3 };
const int VIEW_INFO[3] = { 0, 1, 2 };
//const DirectionTurn DECISIONS[3] = {DirectionTurn::Forward, DirectionTurn::RightTurn, DirectionTurn::LeftTurn};
const Direction DECISIONS[4] = { Direction::Right, Direction::Left, Direction::Up, Direction::Down };
std::random_device rd;
static std::mt19937 rng(rd());



const Vec2D VIEW_VECTORS[8] = {
    {1, 0},
    {1, 1},
    {0, 1},
    {-1, 1},
    {-1, 0},
    {-1, -1},
    {0, -1},
    {1, -1},
};
float ReLU(float x) {
    if (x > 0) {
        return x;
    }
    else {
        return 0;
    }
}

float sigmoid(float x) {
    return 1 / (1 + exp(-x));
}

SnakeViewState AIPlayer::GetView(int(&boardState)[BOARD_PIXEL_COUNT], Board& board) {
    SnakeViewState boardView;
    Vec2D currViewDir;
    Position currPos;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 3; j++) {
            boardView.directions[i][j] = 0;
            int stepsTaken = 1;
            currPos = board.snake.body.front().position;
            currViewDir = VIEW_VECTORS[i];
            while (!Board::IsOnBorder(currPos)) {
                currPos = currPos + currViewDir;

                if (j == 0) {
                    bool onBody = false;
                    for (const auto& body : board.snake.body) {
                        onBody |= body.position == currPos;
                    }
                    if (onBody) {
                        boardView.directions[i][j] = stepsTaken;
                        break;
                    }
                }

                if (j == 1) {
                    if (currPos == board.currentFruit.position) {
                        boardView.directions[i][j] = stepsTaken;
                    }
                }

                if (j == 2) {
                    if (!Board::IsOnBorder(currPos)) {
                        boardView.directions[i][j] = stepsTaken;
                    }
                }

                stepsTaken++;
            }

        }
    }
    return boardView;
}

Direction AIPlayer::GetDecision(int(&boardState)[BOARD_PIXEL_COUNT], Board& board)
{   
    auto view = GetView(boardState, board);
    float layer1Activation[numNeuronsLayer1];
    float layer2Activation[numNeuronsLayer2];
    float outputLayerActivation[numOutputNeurons];
    float currSum;
    for (int i = 0; i < numNeuronsLayer1; i++) {
        currSum = 0;
        for (int j = 0; j < 8 * 3; j++) {
            currSum += (view.directions[i][j] == VIEW_INFO[j] ? 1.0 : 0) * layer1Weight[j];
        }
        currSum += layer1Bias[i];
        layer1Activation[i] = sigmoid(currSum);
    }

    for (int i = 0; i < numNeuronsLayer2; i++) {
        currSum = 0;
        for (int j = 0; j < numNeuronsLayer1; j++) {
            currSum += layer1Activation[j] * layer2Weight[j];
        }
        layer2Activation[i] = sigmoid(currSum);
    }

    float softMaxSum = 0;
    for (int i = 0; i < numOutputNeurons; i++) {
        currSum = 0;
        for (int j = 0; j < numNeuronsLayer2; j++) {
            currSum += layer2Activation[j] * outputLayerWeight[j];
        }
        currSum += outputLayerBias[i];
        outputLayerActivation[i] = sigmoid(currSum);
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
    distMutation = std::normal_distribution<>(0, 1);
    for (auto& weight : layer1Weight) {
        weight = distInit(rng);
    }
    for (auto& weight : layer2Weight) {
        weight = distInit(rng);
    }
    for (auto& weight : outputLayerWeight) {
        weight = distInit(rng);
    }

    for (auto& weight : layer1Bias) {
        weight = distInit(rng);
    }
    for (auto& weight : layer2Bias) {
        weight = distInit(rng);
    }
    for (auto& weight : outputLayerBias) {
        weight = distInit(rng);
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

void AIPlayer::ConvertToChild(const AIPlayer& player1, const AIPlayer& player2)
{   

    RandomiseValues< 8 * 3 * numNeuronsLayer1>(this->layer1Weight, player1.layer1Weight, player2.layer1Weight);
    RandomiseValues< numNeuronsLayer1* numNeuronsLayer2>(this->layer2Weight, player1.layer2Weight, player2.layer2Weight);
    RandomiseValues< numNeuronsLayer2* numOutputNeurons>(this->outputLayerWeight, player1.outputLayerWeight, player2.outputLayerWeight);
    RandomiseValues< numNeuronsLayer1 >(this->layer1Bias, player1.layer1Bias, player2.layer1Bias);
    RandomiseValues< numNeuronsLayer2 >(this->layer2Bias, player1.layer2Bias, player2.layer2Bias);
    RandomiseValues< numOutputNeurons >(this->outputLayerBias, player1.outputLayerBias, player2.outputLayerBias);
}

template<int N>
void AIPlayer::RandomiseValues(std::array<float, N>& toFill, const std::array<float, N>& parent1Values, const std::array<float, N>& parent2Values){
    for (int i = 0; i < N; i++) {
        if (rng() % 2) {
            if (rng() % 3 == 0) {
                toFill[i] = parent1Values[i] + distMutation(rng);
            }
            else {
                toFill[i] = parent1Values[i];
            }
        }
        else {
            if (rng() % 3 == 0) {
                toFill[i] = parent2Values[i] + distMutation(rng);
            }
            else {
                toFill[i] = parent2Values[i];
            }
        }
    }
}
