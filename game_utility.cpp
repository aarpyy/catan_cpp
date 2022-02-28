#include "game_utility.h"

bool encloses(int x, int y, int w, int h, int point_x, int point_y) {
    int a = h / 2;
    int b = w / 2;
    return ((x - b <= point_x) && (x + b >= point_x) && (y - a <= point_y) && (y + a >= point_y));
}

bool encloses(SDL_Rect &rect, SDL_Point &point) {
    return ((rect.x <= point.x) && (rect.x + rect.w >= point.x) && (rect.y <= point.y) && (rect.y + rect.h >= point.y));
}
