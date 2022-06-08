#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "BoardRender.h"
#include "SnakeGame.h"
#include "AIPlayer.hpp"

#define TURNS_DONE_WEIGHT 1
#define FRUITS_EATEN_WEIGHT 200
#define GEN_SIZE 1000
static unsigned int CompileShader(unsigned int type, const std::string& source) {
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "Failed to compile" << (type == GL_VERTEX_SHADER ? "vertex" : "fragment ") << "shader" << std::endl;
        std::cout << message << std::endl;
        glDeleteShader(id);
        return 0;
    }

    return id;
}

static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader) {
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}
int main(void)
{
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


    std::string vertexShader = R"glsl(
    #version 330 core
    
    layout (location = 0) in int state;
    out vec4 color;
    void main(){
        gl_Position =  vec4( ( mod(gl_VertexID, 16.0) / 16.0 * 1.5) - 0.75,
                               ( float(floor(float(gl_VertexID / 16.0) )) / 16.0 * 1.5 ) - 0.75, 0, 1);
        if(state == 1) {
            gl_PointSize = 17;
            color = vec4(1, 0, 0, 1);
        }
        else if(state == 2){
            gl_PointSize = 17;
            color = vec4(1, 1, 0, 1);
        }
        else if(state ==3){
            gl_PointSize = 17;
            color = vec4(0, 1, 1, 1);
        }
        else gl_PointSize = 0;
    }
    )glsl";

    std::string fragmentShader = R"glsl(
    #version 330 core
    
    in vec4 color;
    out vec4 fragColor;

    void main(){
        fragColor = color;
    }
    )glsl";

    unsigned int shader = CreateShader(vertexShader, fragmentShader);
    glUseProgram(shader);
    glEnable(GL_PROGRAM_POINT_SIZE);
    InitRenderer();

    int boardState[BOARD_PIXEL_COUNT];
    int currIndexPoint = 0;

    double timeSinceLastFrame = 1;
    double lastTime = glfwGetTime();
    double currTime = 0;


    Board boardGame = Board();

    int currPlayer = 0;
    std::vector<AIPlayer> players = std::vector<AIPlayer>(GEN_SIZE);
    int secondBestPlayer = 0;
    int bestPlayer = 0;
    int bestScore = 0;
    int currScore = 0;
    int currGen = 0;
    bool neverReachedFruit = true;
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window) && !boardGame.isDead)
    {

        currTime = glfwGetTime();
        timeSinceLastFrame += currTime - lastTime;
        lastTime = currTime;

        //if (timeSinceLastFrame >= 0) {
            /* Render here */
            glClear(GL_COLOR_BUFFER_BIT);

            timeSinceLastFrame = 0;

            // std::cout << "making a new frame" << std::endl;
            //int state;
            //state = glfwGetKey(window, GLFW_KEY_RIGHT);
            //if (state == GLFW_PRESS) boardGame.SetDirectionTurn(DirectionTurn::RightTurn);
            //state = glfwGetKey(window, GLFW_KEY_LEFT);
            //if (state == GLFW_PRESS) boardGame.SetDirectionTurn(DirectionTurn::LeftTurn);


            boardGame.MoveOneFrameForward();
            boardGame.SetStates(boardState);
            if (currPlayer == 0 && currGen % 100 == 0) {
                RenderBoard(boardState);
                glfwSwapBuffers(window);
                
            }
            
            if (boardGame.isDead || (boardGame.turnsSinceLastFruit > 200)) {
                if (currPlayer >= players.size() - 1) {
                    std::cout << "Making Generation " << currGen <<" , using player #" << bestPlayer << "and player #" << secondBestPlayer << std::endl;
                    currGen++;
                    if (bestScore <= 201) {
                        players = std::vector<AIPlayer>(GEN_SIZE);
                    }
                    else {
                        neverReachedFruit = false;
                        for (auto& player : players) {
                            player.ConvertToChild(players[bestPlayer], players[secondBestPlayer]);
                        }
                    }
                    currPlayer = 0;
                    bestPlayer = 0;
                    bestScore = 0;
                }
                else {
                    //std::cout << "Player #" << currPlayer << " died." << std::endl;
                    if (boardGame.FruitsEaten != 0) {
                        //std::cout << " yay" << std::endl;
                    }
                    currScore = boardGame.turnsSinceLastFruit * TURNS_DONE_WEIGHT + boardGame.FruitsEaten * FRUITS_EATEN_WEIGHT;
                    if (currScore >= bestScore) {
                        if (bestScore == 0) {
                            secondBestPlayer = currPlayer;
                        }
                        else {
                            secondBestPlayer = bestPlayer;
                        }
                        
                        bestScore = currScore;
                        bestPlayer = currPlayer;
                    }
                    currPlayer++;
                }
                boardGame = Board();
            }
            else {
                    Direction input = players[currPlayer].GetDecision(boardState, boardGame);

                    boardGame.SetDirection(input);
                 //DirectionTurn input = players[currPlayer].GetDecision(boardState, boardGame);

                 //boardGame.SetDirectionTurn(input);
            }
            
            /*for (auto& state : boardState) {
                state = 0;
            }
            boardState[currIndexPoint++] = 1;*/
            

            /* Swap front and back buffers */
            
        //}
        


        /* Poll for and process events */
        glfwPollEvents();
    }

    std::cout << "You made " << boardGame.turnsDone << " moves and ate " << boardGame.FruitsEaten << " fruits" << std::endl;
    std:getchar();
    glfwTerminate();
    return 0;
}