//
// Created by Andrew Carpenter on 1/21/22.
//

#ifndef CATAN_CPP_GAMEBUTTON_H
#define CATAN_CPP_GAMEBUTTON_H

#include "SDL2"
#include "SDL_ttf/SDL_ttf.h"


class GameButton {

    SDL_Rect *rect;         // Actual SDL_Rect to be rendered
    SDL_Texture *texture;   // Either text or image

public:
    int x, y, w, h;

    GameButton();
    GameButton(int x, int y);
    GameButton(int x, int y, int w, int h);
    GameButton(int x, int y, int w, int h, SDL_Texture *texture);

    // Overrider for delete to also delete rect
    void operator delete(void*);

    void show(SDL_Renderer *renderer);
    void hover(SDL_Renderer *renderer);
    bool encloses(int x, int y);
    bool encloses(SDL_Point &point);

};


#endif //CATAN_CPP_GAMEBUTTON_H
