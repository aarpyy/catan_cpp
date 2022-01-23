//
// Created by Andrew Carpenter on 1/22/22.
//

#include "Game.h"

bool g_express = false;

void checkFlags(int argc, char **argv) {
    std::string expFlag = "-e";
    for (int i = 0; i < argc; i++) {
        if (argv[i] == expFlag) g_express = true;
    }
}

bool runTitleScreen() {
    // Set width and height to exact pixels of title screen image, not resizable
    int w = 565;
    int h = 692;
    SDL_Window *window = SDL_CreateWindow("Settlers of Catan - Start Game",
                                          SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED, w, h, 0);
    if (window == nullptr) {
        SDL_Log("Error initializing window: %s", SDL_GetError());
        return false;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
    if (renderer == nullptr) {
        SDL_Log("Error initializing renderer: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        return false;
    }

    SDL_Texture *background, *startTexture, *loadTexture, *quitTexture;
    startTexture = loadTexture = quitTexture = background = nullptr;

    // Load all textures needed for start screen
    if (!textureFromFile(renderer, &startTexture, "../resources/misc/start_game.bmp")) {
        SDL_DestroyTexture(startTexture);
        SDL_DestroyRenderer(renderer);
        return false;
    }
    if (!textureFromFile(renderer, &loadTexture, "../resources/misc/load_game.bmp")) {
        SDL_DestroyTexture(startTexture);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        return false;
    }
    if (!textureFromFile(renderer, &quitTexture, "../resources/misc/quit_game.bmp")) {
        SDL_DestroyTexture(startTexture);
        SDL_DestroyTexture(loadTexture);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        return false;
    }
    if (!textureFromFile(renderer, &background, "../resources/misc/title_screen.bmp")) {
        SDL_DestroyTexture(startTexture);
        SDL_DestroyTexture(loadTexture);
        SDL_DestroyTexture(quitTexture);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        return false;
    }

    int edge = 8;
    int offset = 180;
    int y = 300;
    int size = 200;

    bool b_start = false;
    bool b_load = true;
    bool b_quit = false;

    // Title screen buttons
    SDL_Rect startButton{edge, y, size, size};
    SDL_Rect loadButton{edge + offset, y, size, size};
    SDL_Rect quitButton{edge + 2 * offset, y, size, size};

    // Add background to renderer
    SDL_RenderCopy(renderer, background, nullptr, nullptr);

    // Add 3 buttons
    SDL_RenderCopy(renderer, startTexture, nullptr, &startButton);
    SDL_RenderCopy(renderer, loadTexture, nullptr, &loadButton);
    SDL_RenderCopy(renderer, quitTexture, nullptr, &quitButton);

    SDL_RenderPresent(renderer);

    SDL_Point mouse;
    SDL_Event event;
    while (!b_start && !b_quit && SDL_WaitEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                return false;
            case SDL_MOUSEBUTTONDOWN:
                if (SDL_GetMouseState(&mouse.x, &mouse.y) & SDL_BUTTON_LMASK) {
                    if (encloses(startButton, mouse)) {
                        b_start = true;
                    } else if (encloses(loadButton, mouse)) {
                        b_load = true;
                    } else if (encloses(quitButton, mouse)) {
                        b_quit = true;
                    }
                }
                break;
        }
    }

    SDL_DestroyTexture(startTexture);
    SDL_DestroyTexture(loadTexture);
    SDL_DestroyTexture(quitTexture);
    SDL_DestroyTexture(background);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    return !b_quit;
}


int main(int argc, char **argv) {

    // Check to see if any commandline arguments
    checkFlags(argc, argv);

    if (SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("SDL_Init() failed: %s", SDL_GetError());
        return 1;
    }

    runTitleScreen();

    return 0;

    GameLoader loader;

    if (!g_express) {
        if (!loader.runPlayerSelect()) {
            return 4;
        }
    } else {
        loader.players.push_back(new Player(1, RUSSET));
        loader.players.push_back(new Player(2, BLUE));
    }

    Game game(&loader);

    SDL_Log("Constructed game");

    if (!game.initialize()) {
        SDL_Log("Error initializing board: %s\n", SDL_GetError());
        return 1;
    }
    SDL_Log("Board initialized");
    if (!game.initializeTTF()) {
        SDL_Log("Error initializing TTF library: %s\n", SDL_GetError());
        return 2;
    }
    if (!game.initializePlayerTextures()) {
        SDL_Log("Error initializing player textures: %s", SDL_GetError());
        return 5;
    }

    SDL_Log("Starting game");
    game.start();
    game.quit();

    return 0;
}
