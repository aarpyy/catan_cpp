#include "GameLoader.h"

#include <chrono>
#include <random>

GameLoader::GameLoader() {
    save_file = "saves/save";
    for (int &i : board) i = 0;
}

bool GameLoader::runPlayerSelect() {
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

void GameLoader::randBoard() {
    /*
     * Generates a random board by iterating through the number of tiles and choosing a random
     * tile to place there, checking first to see if any of those tiles are left to place.
     * Seed is generated at beginning so that it's essentially a random board every time.
     */

    unsigned int seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator(seed);

    // This is essentially Python's randrange(6)
    std::uniform_int_distribution<int> distribution(0, 5);

    // Get copy of constant array of number of each type of resource
    std::array<int, N_RESOURCE> n_tiles{};
    std::copy(nTypes.begin(), nTypes.end(), n_tiles.begin());

    int i, t;
    for (i = 0; i < N_TILES; i++) {

        // Get tile that still has some left to be placed
        t = distribution(generator);
        while (!n_tiles[t]) t = ((t + 1) % N_RESOURCE);

        board[t] |= (1 << i);
        n_tiles[t]--;
    }
}

void GameLoader::save() const {
    std::ofstream outfile(save_file);
    for (const int &i : board) {
        outfile << i << std::endl;
    }
    outfile.close();
}

void GameLoader::quit() const {
    save();
}

