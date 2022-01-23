//
// Created by Andrew Carpenter on 1/21/22.
//

#include "GameButton.h"

GameButton::GameButton(int x, int y, int w, int h, SDL_Texture *texture) {
    this->x = x;
    this->y = y;
    this->w = w;
    this->h = h;
    this->texture = texture;
    this->rect = new SDL_Rect{x, y, w, h};
}

GameButton::GameButton(int x, int y, int w, int h) : GameButton(x, y, w, h, nullptr) { }

GameButton::GameButton(int x, int y) : GameButton(x, y, 0, 0) { }

GameButton::GameButton() : GameButton(0, 0) { }

void *GameButton::operator new(size_t size) {
    return ::operator new(size);
}

void GameButton::operator delete(void *ptr) {
    auto *button = (GameButton*)ptr;
    delete button->rect;
    free(ptr);
}

void GameButton::show(SDL_Renderer *renderer) {
    if (texture != nullptr) {
        SDL_RenderCopy(renderer, texture, nullptr, rect);
    }
    SDL_RenderFillRect(renderer, rect);
}

void GameButton::hover(SDL_Renderer *renderer) {
    show(renderer);
    SDL_Rect mask{x, y, w, h};

    // Set renderer to blend recently shown rect under light gray semi-transparent
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 150, 150, 150, 200);
    SDL_RenderFillRect(renderer, &mask);
}

bool GameButton::encloses(int _x, int _y) const {
    return ((_x >= x) && (_x <= x + w) && (_y >= y) && (_y <= y + h));
}

bool GameButton::encloses(SDL_Point &point) const {
    return encloses(point.y, point.y);
}
