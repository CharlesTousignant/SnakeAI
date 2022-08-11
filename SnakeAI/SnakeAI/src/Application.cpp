#include <Windows.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include "OpenGlSetup.hpp"
#include "BoardRender.hpp"
#include "SnakeGame.hpp"
#include "AIPlayer.hpp"

#include <chrono>
#include <thread>

#define TURNS_DONE_WEIGHT 1
#define FRUITS_EATEN_WEIGHT 200
#define GEN_SIZE 10000

int CalculateScore(const Board& boardGame) {
    return boardGame.turnsSinceLastFruit * TURNS_DONE_WEIGHT + boardGame.FruitsEaten * FRUITS_EATEN_WEIGHT;
}

int ManageAiGame(const AIPlayer& player, GLFWwindow* window = nullptr) {
    int boardState[BOARD_PIXEL_COUNT] = { 0 };
    Board boardGame = Board();
    boardGame.SetStates(boardState);


    while (! (boardGame.isDead || (boardGame.turnsSinceLastFruit > 200) )) {
        Direction input = player.GetDecision(boardGame);
        boardGame.SetDirection(input);
        boardGame.MoveOneFrameForward();
        boardGame.SetStates(boardState);
        if (window) {
            // std::cout << input << ", " << CalculateScore(boardGame) << "\n";
            glClear(GL_COLOR_BUFFER_BIT);
            RenderBoard(boardState);
            glfwSwapBuffers(window);
            
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
    return CalculateScore(boardGame);
}


int main(void)
{   

    std::ofstream txtResults("TrainResults.txt");
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(1080, 1080, "Snek", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK) {
        std::cout << "Error!" << std::endl;
    }

    std::cout << glGetString(GL_VERSION) << std::endl;
    SetupOpenGl();
    InitRenderer();

    int boardState[BOARD_PIXEL_COUNT];


    Board boardGame = Board();


    std::vector<AIPlayer> players = std::vector<AIPlayer>(GEN_SIZE);

    int currPlayer = 0;
    int currScore = 0;
    int currGen = 0;

    int secondBestPlayer = 0;
    int bestPlayer = 0;
    int bestScore = 0;

    boardGame.SetStates(boardState);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {   
        while (currPlayer <= GEN_SIZE - 1) {

            auto score = ManageAiGame(players[currPlayer]);

            if (score >= bestScore) {
                if (bestScore == 0) {
                    secondBestPlayer = currPlayer;
                }
                else {
                    secondBestPlayer = bestPlayer;
                }

                bestScore = score;
                bestPlayer = currPlayer;
            }
            currPlayer++;
        }
        txtResults << "Generation # " << currGen << ", score of " << bestScore << "\n";
      
        // Show the best player's performance 
        auto scoreReplay = ManageAiGame(players[bestPlayer], window);
        std::cout << "Player# " << bestPlayer << ", score of:" << scoreReplay << "\n";
        if (scoreReplay != bestScore) {
            std::cout << "Replay didn't get best player score \n";
        }

        for (auto& player : players) {
            player.ConvertToChild(players[bestPlayer], players[secondBestPlayer]);
        }
        currPlayer = 0;
        bestPlayer = 0;
        bestScore = 0;
        currGen++;
        /* Poll for and process events */
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}