#ifndef CATAN_GAME_UTILITY_H
#define CATAN_GAME_UTILITY_H

#endif //CATAN_GAME_UTILITY_H

#include <SDL.h>
#include <SDL_ttf.h>

bool encloses(int x, int y, int w, int h, int point_x, int point_y);
bool encloses(SDL_Rect &rect, SDL_Point &point);

bool surfaceFromFile(SDL_Surface **surface, const char *file);
bool textureFromFile(SDL_Renderer *renderer, SDL_Texture **texture, const char *file);
