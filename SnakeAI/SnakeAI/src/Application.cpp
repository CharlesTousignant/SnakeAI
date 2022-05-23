#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "BoardRender.h"
#include "SnakeGame.h"

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

    GLuint buffer;

    std::string vertexShader = R"glsl(
    #version 330 core
    
    layout (location = 0) in int state;
    void main(){
          gl_Position =  vec4( ( float(mod(gl_VertexID, 64.0)) / 64.0 * 1.5) - 0.75,
                               ( float(floor(float(gl_VertexID / 64.0) )) / 64.0 * 1.5 ) - 0.75, 0, 1);
        if(state == 1) gl_PointSize = 8;
        else gl_PointSize = 0;
    }
    )glsl";

    std::string fragmentShader = R"glsl(
    #version 330 core

    out vec4 fragColor;

    void main(){
        fragColor = vec4(1.0, 0, 0, 1);
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
    int state;
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window) && !boardGame.isDead)
    {

        currTime = glfwGetTime();
        timeSinceLastFrame += currTime - lastTime;
        lastTime = currTime;
        if (timeSinceLastFrame >= 0.25) {
            /* Render here */
            glClear(GL_COLOR_BUFFER_BIT);

            timeSinceLastFrame = 0;

            // std::cout << "making a new frame" << std::endl;


            state = glfwGetKey(window, GLFW_KEY_RIGHT);
            if (state == GLFW_PRESS) boardGame.SetDirection(Direction::Right);
            state = glfwGetKey(window, GLFW_KEY_LEFT);
            if (state == GLFW_PRESS) boardGame.SetDirection(Direction::Left);
            state = glfwGetKey(window, GLFW_KEY_UP);
            if (state == GLFW_PRESS) boardGame.SetDirection(Direction::Up);
            state = glfwGetKey(window, GLFW_KEY_DOWN);
            if (state == GLFW_PRESS) boardGame.SetDirection(Direction::Down);

            boardGame.MoveOneFrameForward();
            boardGame.SetStates(boardState);

            /*for (auto& state : boardState) {
                state = 0;
            }
            boardState[currIndexPoint++] = 1;*/
            RenderBoard(boardState);

            /* Swap front and back buffers */
            glfwSwapBuffers(window);
        }
        


        /* Poll for and process events */
        glfwPollEvents();
    }

    std::cout << "You made " << boardGame.turnsDone << " moves and ate " << boardGame.FruitsEaten << " fruits" << std::endl;
    std:getchar();
    glfwTerminate();
    return 0;
}