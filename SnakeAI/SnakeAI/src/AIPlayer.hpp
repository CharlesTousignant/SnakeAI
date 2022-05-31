#pragma once
#include "SnakeGame.h"

#define numNeuronsLayer1 10
#define numNeuronsLayer2 10
#define numOutputNeurons 4
class AIPlayer {
public:
    Direction GetDecision(int(&boardState)[BOARD_PIXEL_COUNT]);
    AIPlayer();



    float layer1Weight[BOARD_PIXEL_COUNT * 3 * 10]; 
    float layer2Weight[10 * 10];
    float outputLayerWeight[10 * 4];

    float layer1Bias[10];
    float layer2Bias[10];
    float outputLayerBias[4];
};