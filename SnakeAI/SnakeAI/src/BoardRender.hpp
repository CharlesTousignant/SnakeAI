#pragma once

#define BOARD_DIMENSIONS 16
#define BOARD_PIXEL_COUNT BOARD_DIMENSIONS * BOARD_DIMENSIONS

void InitRenderer();

void RenderBoard(const int (state)[BOARD_PIXEL_COUNT]);