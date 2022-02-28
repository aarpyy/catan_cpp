#ifndef CATAN_GAME_UTILITY_H
#define CATAN_GAME_UTILITY_H

#endif //CATAN_GAME_UTILITY_H

#include "SDL/include/SDL.h"
#include "SDL_ttf/SDL_ttf.h"

bool encloses(int x, int y, int w, int h, int point_x, int point_y);
bool encloses(SDL_Rect &rect, SDL_Point &point);
