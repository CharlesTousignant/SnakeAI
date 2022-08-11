#include "AIPlayer.hpp"
#include <math.h>

//const int STATES[3] = { 1, 2, 3 };
const int VIEW_INFO[3] = { 0, 1, 2 };
//const DirectionTurn DECISIONS[3] = {DirectionTurn::Forward, DirectionTurn::RightTurn, DirectionTurn::LeftTurn};
const Direction DECISIONS[4] = { Direction::Right, Direction::Left, Direction::Up, Direction::Down };
static std::mt19937 rng;



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
double ReLU(double x) {
    if (x > 0) {
        return x;
    }
    else {
        return 0;
    }
}

double sigmoid(double x) {
    return 1 / (1 + exp(std::max(-10.0, std::min( - x, 10.0))));
}

SnakeViewState AIPlayer::GetView( Board& board) const {
    SnakeViewState boardView{};
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

Direction AIPlayer::GetDecision(Board& board) const
{   
    auto view = GetView( board);
    double layer1Activation[numNeuronsLayer1] = {};
    double layer2Activation[numNeuronsLayer2] = {};
    double outputLayerActivation[numOutputNeurons] = {};
    double currSum = 0;
    for (int i = 0; i < numNeuronsLayer1; i++) {
        currSum = 0;
        for (int j = 0; j < 8 * 3; j++) {
            currSum += (view.directions[i][j] == VIEW_INFO[j] ? 1 : 0) * layer1Weight[j];
        }
        currSum += layer1Bias[i];
        layer1Activation[i] = sigmoid(currSum);
    }

    for (int i = 0; i < numNeuronsLayer2; i++) {
        currSum = 0;
        for (int j = 0; j < numNeuronsLayer1; j++) {
            currSum += layer1Activation[j] * layer2Weight[j];
        }
        currSum += layer2Bias[i];
        layer2Activation[i] = sigmoid(currSum);
    }

    for (int i = 0; i < numOutputNeurons; i++) {
        currSum = 0;
        for (int j = 0; j < numNeuronsLayer2; j++) {
            currSum += layer2Activation[j] * outputLayerWeight[j];
        }
        currSum += outputLayerBias[i];
        outputLayerActivation[i] = sigmoid(currSum);
    }

    int highestOutputIndex = 0;
    double highestActivationFound = 0;
    double currentActivation = 0;
    for (int i = 0; i < numOutputNeurons; i++) {
        currentActivation = outputLayerActivation[i];
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
    for (double& weight : layer1Weight) {
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
void AIPlayer::RandomiseValues(std::array<double, N>& toFill, const std::array<double, N>& parent1Values, const std::array<double, N>& parent2Values){
    for (int i = 0; i < N; i++) {
        if (!((rng() % 3) == 0)) {
            if (rng() % 10) {
                toFill[i] = parent1Values[i] + distMutation(rng);
            }
            else {
                toFill[i] = parent1Values[i];
            }
        }
        else {
            if (rng() % 10) {
                toFill[i] = parent2Values[i] + distMutation(rng);
            }
            else {
                toFill[i] = parent2Values[i];
            }
        }
    }
}
