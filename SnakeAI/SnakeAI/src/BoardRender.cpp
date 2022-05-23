#include "BoardRender.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
GLuint buffer;

GLuint bufferPos;


int states[3] = {
    0, 1, 1
};


void InitRenderer()
{

    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, BOARD_PIXEL_COUNT * sizeof(int), NULL, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribIPointer(0, 1, GL_INT, sizeof(int), 0);
}

void RenderBoard(const int(&state)[BOARD_PIXEL_COUNT])
{
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, BOARD_PIXEL_COUNT * sizeof(int), state, GL_DYNAMIC_DRAW);
    //glEnableVertexAttribArray(0);
    //glVertexAttribPointer(0, 1, GL_INT, GL_FALSE, sizeof(int), 0);

    glDrawArrays(GL_POINTS, 0, BOARD_PIXEL_COUNT);
}
 