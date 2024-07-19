#include "raylib.h"
#include <stdbool.h>
#define GAME_H

int rows = 10;
int columns = 10;
const float TILE_SIZE_CONSTANT = 600.0;
int bombs_amt = 15;
int tiles_revealed = 0;
bool lost = false;
Camera2D camera;

Vector2 GlobalMousePosition();
