#include "raylib.h"
#include <stdbool.h>
#define GAME_H

int rows = 10;
int columns = 10;
const float TILE_SIZE_CONSTANT = 600.0;
int bombs_amt = 20;
int remaining_bombs = 20;
int tiles_revealed = 0;
bool lost = false;
Camera2D camera;

Vector2 GlobalMousePosition();
