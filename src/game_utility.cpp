#include "game_utility.h"

bool encloses(int x, int y, int w, int h, int point_x, int point_y) {
    int a = h / 2;
    int b = w / 2;
    return ((x - b <= point_x) && (x + b >= point_x) && (y - a <= point_y) && (y + a >= point_y));
}

bool encloses(SDL_Rect &rect, SDL_Point &point) {
    return ((rect.x <= point.x) && (rect.x + rect.w >= point.x) && (rect.y <= point.y) && (rect.y + rect.h >= point.y));
}

bool surfaceFromFile(SDL_Surface **surface, const char *file) {
    *surface = SDL_LoadBMP(file);
    if (*surface == nullptr) {
        SDL_Log("Unable to locate %s; SDL error: %s", file, SDL_GetError());
        return false;
    } else {
        return true;
    }
}

bool textureFromFile(SDL_Renderer *renderer, SDL_Texture **texture, const char *file) {
    SDL_Surface *surface = nullptr;
    if (!surfaceFromFile(&surface, file)) {
        return false;
    } else {
        *texture = SDL_CreateTextureFromSurface(renderer, surface);
        if (*texture == nullptr) {
            SDL_Log("Failed to create texture from %s: %s", file, SDL_GetError());
            return false;
        } else {
            SDL_FreeSurface(surface);
            return true;
        }
    }

}
