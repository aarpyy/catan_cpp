#include "GameLoader.h"

GameLoader::GameLoader() {
    save_file = "saves/save";
    board = nullptr;
}

bool GameLoader::runTitleScreen() {
    bool start = false;
    bool load = true;

    if (SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Error initializing title screen: %s", SDL_GetError());
        return false;
    }

    // Set width and height to exact pixels of title screen image, not resizable
    int w = 565;
    int h = 692;
    SDL_Window *window = SDL_CreateWindow("",
                                          SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED, w, h, 0);

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_Surface *image = SDL_LoadBMP("resources/misc/title_screen.bmp");
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, image);
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);

    int edge = 8;
    int offset = 180;

    // Title screen buttons
    SDL_Rect startGameButton = {edge, 300, 200, 200};
    SDL_Rect loadGameButton = {edge + offset, 300, 200, 200};
    SDL_Rect quitGameButton = {edge + 2 * offset, 300, 200, 200};

    SDL_Surface *startGameImage = SDL_LoadBMP("resources/misc/start_game.bmp");
    SDL_Surface *loadGameImage = SDL_LoadBMP("resources/misc/load_game.bmp");
    SDL_Surface *quitGameImage = SDL_LoadBMP("resources/misc/quit_game.bmp");

    SDL_Texture *startGameTexture = SDL_CreateTextureFromSurface(renderer, startGameImage);
    SDL_Texture *loadGameTexture = SDL_CreateTextureFromSurface(renderer, loadGameImage);
    SDL_Texture *quitGameTexture = SDL_CreateTextureFromSurface(renderer, quitGameImage);

    SDL_RenderCopy(renderer, startGameTexture, nullptr, &startGameButton);
    SDL_RenderCopy(renderer, loadGameTexture, nullptr, &loadGameButton);
    SDL_RenderCopy(renderer, quitGameTexture, nullptr, &quitGameButton);

    SDL_RenderPresent(renderer);

    SDL_Point mouse;
    SDL_Event event;
    while (!start && SDL_WaitEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                return false;
            case SDL_MOUSEBUTTONDOWN:
                SDL_GetMouseState(&mouse.x, &mouse.y);
                if (encloses(startGameButton, mouse)) {
                    start = true;
                } else if (encloses(loadGameButton, mouse)) {
                    load = true;
                } else if (encloses(quitGameButton, mouse)) {
                    return false;
                }
                break;
        }
    }

    SDL_DestroyTexture(startGameTexture);
    SDL_DestroyTexture(loadGameTexture);
    SDL_DestroyTexture(quitGameTexture);
    SDL_DestroyTexture(texture);

    SDL_FreeSurface(startGameImage);
    SDL_FreeSurface(loadGameImage);
    SDL_FreeSurface(quitGameImage);
    SDL_FreeSurface(image);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    board = new Board();
    return true;
}

bool GameLoader::getPlayerInfo() {
    int w = 565;
    int h = 692;

    SDL_Window *window = SDL_CreateWindow("",
                                          SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w, h, 0);

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    SDL_Rect color1{0, 420, 113, 272};
    SDL_Rect color2{113, 420, 113, 272};
    SDL_Rect color3{226, 420, 113, 272};
    SDL_Rect color4{339, 420, 113, 272};
    SDL_Rect color5{452, 420, 113, 272};
    std::array<SDL_Rect, 5> colorRect{color1, color2, color3, color4, color5};
    std::array<bool, 5> chosen{false, false, false, false, false};


    SDL_Surface *rumSurface = SDL_LoadBMP("resources/misc/color_1.bmp");
    SDL_Surface *russetSurface = SDL_LoadBMP("resources/misc/color_2.bmp");
    SDL_Surface *blueSurface = SDL_LoadBMP("resources/misc/color_3.bmp");
    SDL_Surface *greenSurface = SDL_LoadBMP("resources/misc/color_4.bmp");
    SDL_Surface *yellowSurface = SDL_LoadBMP("resources/misc/color_5.bmp");

    std::array<SDL_Texture*, 5> colorChosenScreen{SDL_CreateTextureFromSurface(renderer, rumSurface),
                                                  SDL_CreateTextureFromSurface(renderer, russetSurface),
                                                  SDL_CreateTextureFromSurface(renderer, blueSurface),
                                                  SDL_CreateTextureFromSurface(renderer, greenSurface),
                                                  SDL_CreateTextureFromSurface(renderer, yellowSurface)};

    SDL_FreeSurface(rumSurface);
    SDL_FreeSurface(russetSurface);
    SDL_FreeSurface(blueSurface);
    SDL_FreeSurface(greenSurface);
    SDL_FreeSurface(yellowSurface);

    SDL_Surface *chooseColorSurface = SDL_LoadBMP("resources/misc/choose_color_1.bmp");
    SDL_Surface *chooseContinueSurface = SDL_LoadBMP("resources/misc/choose_color_2.bmp");
    SDL_Texture *chooseColorTexture = SDL_CreateTextureFromSurface(renderer, chooseColorSurface);
    SDL_Texture *chooseContinueTexture = SDL_CreateTextureFromSurface(renderer, chooseContinueSurface);

    SDL_Texture *currentTexture = chooseColorTexture;

    SDL_RenderCopy(renderer, currentTexture, nullptr, nullptr);
    SDL_RenderPresent(renderer);

    bool done = false;
    bool continueGame = false;

    SDL_Event event;
    SDL_Point mouse;

    int nPlayers = 0;

    SDL_SetRenderDrawColor(renderer, 150, 150, 150, 200);
    while (!done && SDL_WaitEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                return false;
            case SDL_MOUSEBUTTONDOWN:
                SDL_GetMouseState(&mouse.x, &mouse.y);
                for (int i = 0; i < 5; i++) {
                    if (encloses(colorRect[i], mouse)) {
                        if (!chosen[i]) {
                            chosen[i] = true;

                            SDL_RenderCopy(renderer, colorChosenScreen[i], nullptr, nullptr);
                            SDL_RenderPresent(renderer);
                            SDL_Delay(1900);

                            players.push_back(new Player(i, allColors[i]));
                            nPlayers++;
                            if (nPlayers == 2) {
                                currentTexture = chooseContinueTexture;
                            } else if (nPlayers == 5) {
                                continueGame = done = true;
                            }

                            SDL_RenderCopy(renderer, currentTexture, nullptr, nullptr);
                            for (int j = 0; j < 5; j++) {
                                if (chosen[j]) {
                                    SDL_RenderFillRect(renderer, &colorRect[j]);
                                }
                            }
                            SDL_RenderPresent(renderer);

                            // If the user clicked anywhere during that screen, ignore it
                            SDL_PumpEvents();
                            SDL_FlushEvent(SDL_MOUSEBUTTONDOWN);
                        }
                        break;
                    } else if (nPlayers >= 2 && ((163 <= mouse.x) && (393 >= mouse.x)
                                                    && (72 <= mouse.y) && (184 >= mouse.y))) {
                        continueGame = done = true;
                        break;
                    }
                }
        }
    }

    if (continueGame) {
        // Delay for a bit so its not a weird transition into new image
        SDL_Delay(1000);
        SDL_Surface *transitionSurface = SDL_LoadBMP("resources/misc/transition.bmp");
        SDL_Texture *transitionTexture = SDL_CreateTextureFromSurface(renderer, transitionSurface);
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, transitionTexture, nullptr, nullptr);

        SDL_RenderPresent(renderer);

        SDL_Delay(3000);

        SDL_DestroyTexture(transitionTexture);
        SDL_FreeSurface(transitionSurface);
    }

    SDL_DestroyTexture(chooseColorTexture);
    SDL_FreeSurface(chooseColorSurface);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    return continueGame;
}

void GameLoader::save() const {
    std::ofstream outfile(save_file);
    std::array<int, N_RESOURCE> bit_board = board->getBitBoard();
    for (int& i : bit_board) {
        outfile << i << std::endl;
    }
    outfile.close();
}

void GameLoader::saveAndClose() const {
    save();
    board->freeBoard();
    delete board;
}

